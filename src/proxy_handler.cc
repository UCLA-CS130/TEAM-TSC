#include "proxy_handler.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>
#include <boost/log/trivial.hpp>
#include <unordered_map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <boost/filesystem.hpp>

namespace http{
namespace server{

ProxyHandler::ProxyHandler():
  RequestHandler(),
  io_service_()
{
}

RequestHandler::Status
ProxyHandler::Init(const std::string& uri_prefix, 
                   const NginxConfig& config)
{
  uri_prefix_ = uri_prefix;
  for (auto statement: config.statements_) {
    std::string start_token = statement->tokens_[0];
    if (start_token == "host") {
      if (statement->tokens_.size() != 2) {
        return RequestHandler::invalid_config;
      }
      else {
        server_host_ = statement->tokens_[1];
      }
    }
    else if (start_token == "port") {
      if (statement->tokens_.size() != 2) {
        return RequestHandler::invalid_config;
      }
      else {
        server_port_ = statement->tokens_[1];
        for (size_t i = 0; i < server_port_.size(); i++){
		  if (!std::isdigit(server_port_[i])){
		    return RequestHandler::invalid_config;
		  }
		}
      }
    }
  }
  if ((server_host_ == "") || (server_port_ == "")) {
  	return RequestHandler::invalid_config;
  }
  return RequestHandler::ok;
}

using boost::asio::ip::tcp;
	  
RequestHandler::Status 
ProxyHandler::HandleRequest(const Request& request, Response* response)
{
  std::string request_uri = request.uri();
  std::size_t findLoc = request_uri.find(uri_prefix_);
  if(findLoc != std::string::npos) {
    request_uri.erase(findLoc, uri_prefix_.size());
  }
  if (request_uri == "" || request_uri[0] != '/') request_uri = "/" + request_uri;

  // If the request if cached, used the cached version instead
  if (HandleCache(request_uri, response)) return RequestHandler::ok;

  // If the request contains like 'If-not-modified' 

  std::string server_host = server_host_;
  std::string server_port = server_port_;
  std::string uri = request_uri;

  while(true) 
  {
  	response->Clear();
  	boost::system::error_code ec;
  	tcp::resolver resolver(io_service_);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve({server_host, server_port}, ec);
    if (ec) return HandleError("resolve", ec);

    tcp::socket socket(io_service_);
    boost::asio::connect(socket, endpoint_iterator, ec);
    if (ec) return HandleError("connect", ec);

	  boost::asio::streambuf requestBuf;
    std::ostream request_stream(&requestBuf);

    // We construct the new request based on the intial request to handle the following situation
    // The browser with 'no-cache' send the 'etag'
    std::string raw_request = MakeRequest(request, uri, server_host, server_port);
    request_stream << raw_request;

    // Send the request.
    boost::asio::write(socket, requestBuf, ec);
    if (ec) return HandleError("write", ec);

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf resp;
    boost::asio::read_until(socket, resp, "\r\n", ec);
    if (ec) return HandleError("read_until", ec);

    std::string raw_response = "";
    // Check that response is OK.
    std::istream response_stream(&resp);
    std::string http_version;
    response_stream >> http_version;

    unsigned int status_code;
    response_stream >> status_code;

    std::string status_message;
    std::getline(response_stream, status_message);
    
    raw_response += http_version + " " + std::to_string(status_code) + status_message + "\n";

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, resp, "\r\n\r\n", ec);
    if (ec) return HandleError("read_until", ec);

    // Process the response headers.
    bool make_cache = false;
    boost::posix_time::ptime expire;
    std::string e_tag = "";

    std::string header;
    while (std::getline(response_stream, header) && header != "\r")
    {
      header.pop_back();
  	  std::string head = header.substr(0, header.find(":"));
  	  std::string value = header.substr(header.find(":") + 2);


      if (status_code != 302 && 
          status_code != 304 &&
          head == "Cache-Control" && 
          value.find("no-store") == std::string::npos &&
  	      value.find("no-cache") == std::string::npos) 
      {
  	    if(value.find("max-age") != std::string::npos) 
        {
          std::size_t start_of_max_age = value.find("max-age");
          std::size_t end_of_max_age = value.find(",", start_of_max_age);
          std::string max_age_str = value.substr(start_of_max_age, end_of_max_age - start_of_max_age);
          std::string cache_life = max_age_str.substr(8);

  	      boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
  	      boost::posix_time::time_duration cache_duration(0, 0, std::stoi(cache_life), 0);
  	      boost::posix_time::ptime expiration = now + cache_duration;
  	      if(std::stoi(cache_life) > 0) 
          {
            make_cache = true;
            expire = expiration;
  	      }
  	    }
    	}
      else if (status_code != 302 &&
               status_code != 304 &&
               head == "ETag")
      {
        e_tag = value;
      }
  	  raw_response += header + "\n";

  	  if (status_code == 302 && head.compare("Location")==0) 
      {
    		// absolute_path: Location: http(s)://host(:port)/url(?...)
    		// relative_path: Location: /url(?...)
    		ParseLocation(value, server_host, server_port, uri);
    		break;
    	}
    }
    if (status_code == 302) continue;
    raw_response += "\r\n";

    // Read until EOF, writing data to output as we go.
    std::ostringstream ss;
  	ss << &resp;
    raw_response += ss.str();

    while (boost::asio::read(socket, resp, ec)) {
      std::ostringstream ss;
      ss << &resp;
      raw_response += ss.str();
      if (ec) {
        break;
      }
    }
    
    if (ec != boost::asio::error::eof) {
      response->Clear();
      response->SetStatus(Response::internal_server_error);
      return RequestHandler::handle_fail;
    }

    if(status_code != 302) {
      response->SetRawResponse(raw_response);
      if (make_cache) MakeCache(request_uri, expire, e_tag, response);
      break;
    }
  }
  return RequestHandler::ok;
}

std::string 
ProxyHandler::MakeRequest(const Request& request,
                          const std::string& uri,
                          const std::string& server_host,
                          const std::string& server_port)
{
  std::string raw_request = "";
  raw_request += request.method() + " " + uri + " " + request.version() + "\r\n";
  Request::Headers headers = request.headers();
  bool has_host = false, has_connection = false;
  for (auto header: headers) {
    if (header.first == "Host") {
      raw_request += "Host: " + server_host + ":" + server_port + "\r\n";
      has_host = true;
    }
    else if (header.first == "Connection") {
      raw_request += "Connection: close\r\n";
      has_connection = true;
    }
    else raw_request += header.first + ": " + header.second + "\r\n";
  }
  if (!has_host) raw_request += "Host: " + server_host + ":" + server_port + "\r\n";
  if (!has_connection) raw_request += "Connection: close\r\n";
  raw_request += "\r\n";
  raw_request += request.body();
  return raw_request;
}

void 
ProxyHandler::MakeCache(const std::string& uri, 
                        boost::posix_time::ptime expiration,
                        std::string e_tag,
                        Response *response)
{
  std::ofstream cache_file;
  BOOST_LOG_TRIVIAL(info) << "MAKING CACHE\n";
  std::string path = uri;

  // creating the directory
  if(path.back() == '/') {
    path = path + "index";
  }
  std::size_t find_start = 1;
  std::size_t dir_end = path.find("/", find_start);
  std::string directory;
  while (dir_end != std::string::npos)
  {
    std::string directory = path.substr(0, dir_end);
    if(!(boost::filesystem::exists(boost::filesystem::system_complete("./proxy_cache" + directory)))) 
    {
      boost::filesystem::create_directory(boost::filesystem::system_complete("./proxy_cache" + directory));
    }
    find_start = dir_end + 1;
    dir_end = path.find("/", find_start);
  }

  // writing to the file
  cache_file.open("proxy_cache" + path);
  BOOST_LOG_TRIVIAL(info) << "Write to cache file for uri= " << uri << "\n";
  cache_file << response->ToString();
  cache_file.close();

  // adding to the map
  cache_[path] = cache_sign(expiration, e_tag);
}

bool
ProxyHandler::HandleCache(const std::string& uri,
                          Response *response)
{
  std::string path = uri;
  if(path.back() == '/') {
    path = path + "index";
  }
  if (cache_.find(path) == cache_.end())
    return false;

  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
  if (now >= cache_[path].expiration_) {
    if (!UpdateCache(uri, path)) {
      return false;
    }
  }

  /*************************************************************************************/
  /* Maybe the request contains 'If-None-Match'                                        */
  /* For this situation, we just send back the contents with 200, and the new max-age. */
  /* The better solution is sending back 304 to clients.                               */
  /*************************************************************************************/
  BOOST_LOG_TRIVIAL(info) << "Using cache\n";
  response->SetBody("");
  std::ifstream cached_response;
  cached_response.open("proxy_cache" + path);
  BOOST_LOG_TRIVIAL(info) << "proxy_cache" << uri;

  std::string raw_response = "";
  std::string line;
  while(!cached_response.eof()) {
    std::getline(cached_response, line);
    if(line == "\r") {
      raw_response += "\r\n";
      break;
    }
    if (line.find("Date:") != std::string::npos) {
      static char const* const fmt = "%a, %d, %b, %Y, %H:%M:%S GMT";
      std::ostringstream sstream;
      sstream.imbue(std::locale(std::cout.getloc(), new boost::posix_time::time_facet(fmt)));
      sstream << boost::posix_time::second_clock::universal_time();
      line = "Date: " + sstream.str() + "\r";
    }
    // update the max-age
    if (line.find("Cache-Control") != std::string::npos) {
      boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
      boost::posix_time::ptime expiration = cache_[path].expiration_;
      boost::posix_time::time_duration max_age = expiration - now;
      std::string max_age_sec = std::to_string(max_age.total_milliseconds() / 1000);

      std::string value = line.substr(line.find(": ") + 2);
      std::size_t token_start = 0;
      line = "Cache-Control: ";
      while(true) {
        std::size_t token_end = value.find(",", token_start);
        if (token_end == std::string::npos) {
          std::string token = value.substr(token_start);
          if (token.find("max-age") != std::string::npos) {
            line += "max-age=" + max_age_sec + "\r";
          }
          else line += token;
          break;
        }
        std::string token = value.substr(token_start, token_end - token_start + 1);
        if (token.find("max-age") != std::string::npos) {
          line += "max-age=" + max_age_sec + ",";
        }
        else line += token;
        token_start = token_end + 1; 
      }

    }
    raw_response += line + "\n";
  }
  //Read in the body
  std::stringstream buff;
  buff << cached_response.rdbuf();
  raw_response += buff.str();
  response->SetRawResponse(raw_response);
  cached_response.close();
  return true;
}

bool
ProxyHandler::UpdateCache(const std::string& uri, const std::string& path)
{
  BOOST_LOG_TRIVIAL(trace) << "Update the Cache";
  Request request;
  request.SetMethod("GET");
  request.SetUri(uri);
  request.SetVersion("HTTP/1.1");
  request.AddHeader("Host", server_host_ + ":" + server_port_);
  request.AddHeader("Accept", "*/*");
  request.AddHeader("If-None-Match", cache_[path].e_tag_);
  request.AddHeader("Connection", "close");
  Response response;

  cache_.erase(path);

  RequestHandler::Status status = HandleRequest(request, &response);
  if (status != RequestHandler::ok) return false;

  // update the cache
  std::string raw_response = response.ToString();
  std::stringstream ss(raw_response);
  std::string response_line;
  std::getline(ss, response_line);
  // if receives a 302 response
  if (response_line.find("304") != std::string::npos) {
    // get the 'cache-control' and 'expire' part
    std::string header;
    boost::posix_time::ptime expire;
    std::string e_tag;
    while(std::getline(ss, header) && header != "\r") {
      if (header.find("Cache-Control") != std::string::npos) {
        if(header.find("max-age") != std::string::npos) 
        {
          std::size_t start_of_max_age = header.find("max-age");
          std::size_t end_of_max_age = header.find(",", start_of_max_age);
          std::string max_age_str = header.substr(start_of_max_age, end_of_max_age - start_of_max_age);
          std::string cache_life = max_age_str.substr(8);
          
          boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
          boost::posix_time::time_duration cache_duration(0, 0, std::stoi(cache_life), 0);
          boost::posix_time::ptime expiration = now + cache_duration;
          if(std::stoi(cache_life) > 0) 
          {
            expire = expiration;
          }
        }
        else return false;
      }
      if (header.find("Etag") != std::string::npos) {
        e_tag = header.substr(header.find(": ") + 2);
      }
      /*if (header.find("Expires") != std::string::npos) {
        expire = header;
      }*/
    }
    // update the 'cache-control' in the cache-file
    /*****************************************************************************************/
    /* Here I do a trick thing.                                                              */
    /* Since the max-age usually will not change. So I don'y update the cache-control header.*/
    /* Instead I just change the cache_[path].first (expiration) and cache_[path].second     */
    /*****************************************************************************************/
    cache_[path] = cache_sign(expire, e_tag);
    return true;
  }
  else if (response_line.find("200") != std::string::npos) {
    /***************************************************************************************/
    /* Mention Here:                                                                       */
    /* We have already updated the cache files in HandleRequest if the status_code is 200! */
    /***************************************************************************************/
    return true;
  }
  else {
    return false;
  }
}

RequestHandler::Status
ProxyHandler::HandleError(const std::string& error_info, const boost::system::error_code& ec) 
{
  BOOST_LOG_TRIVIAL(trace) << "ProxyHandler: " << error_info << " " << "error_code=" << ec;
  return RequestHandler::handle_fail;
}

void 
ProxyHandler::ParseLocation(const std::string& location_header,
	   						std::string& host,
	   						std::string& port,
	   						std::string& path)
{
  // absolute url
  if (location_header.find("://") != std::string::npos) {
    std::size_t host_start = location_header.find("://") + 3;
    std::size_t path_start = location_header.find("/", host_start);
    std::string host_and_port;
    // process host and port
    if (path_start == std::string::npos) host_and_port = location_header.substr(host_start);
    else host_and_port = location_header.substr(host_start, path_start - host_start);
    std::size_t delimeter_pos = host_and_port.find(":");
    if (delimeter_pos == std::string::npos 
        || delimeter_pos == host_and_port.size() - 1) {
      host = host_and_port;
      port = "80";
    }
    else {
      host = host_and_port.substr(0, delimeter_pos);
      port = host_and_port.substr(delimeter_pos + 1);
    }
    // process relative_url
    if (path_start == std::string::npos) {
      path = "/";
    }
    else path = location_header.substr(path_start);
  } // relative url
  else {
    path = location_header;
    if (path.empty() || path[0] != '/') path = "/" + path;
  }
}

} // server
} // http
