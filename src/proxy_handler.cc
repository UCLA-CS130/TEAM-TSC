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
  std::string server_host = server_host_;
  std::string server_port = server_port_;
  std::string uri = request.uri();
  std::size_t findLoc = uri.find(uri_prefix_);

  //If the request if cached, used the cached version instead
  if(cache_.find(request.uri()) != cache_.end()) {
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    if(now < cache_[request.uri()]) {
      BOOST_LOG_TRIVIAL(info) << "Using cache\n";
      response->SetBody("");
      std::ifstream cached_response("server_cache/"+request.uri());
      while(!cached_response.eof()) {
	std::string line;
	std::getline(cached_response, line);
	if(line.find("Date:") != std::string::npos) {
	  static char const* const fmt = "%a, %d, %b, %Y, %H:%M:%S GMT";
	  std::ostringstream sstream;
	  sstream.imbue(std::locale(std::cout.getloc(), new boost::posix_time::time_facet(fmt)));
	  sstream << boost::posix_time::second_clock::universal_time();
	  line = "Date: " + sstream.str();
	}
	response->AppendBody(line);
      }
      cached_response.close();
      return RequestHandler::ok;
    }
  }
  if(findLoc != std::string::npos) {
	uri.erase(findLoc, uri_prefix_.size());
  }
  if (uri == "" || uri[0] != '/') uri = "/" + uri;

  std::ofstream cache_file;
  bool is_cache = false;
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
    request_stream << request.method() << " " << uri << " HTTP/1.1\r\n";
    request_stream << "Host: " << server_host << ":" << server_port << "\r\n"; 
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";
    if (!request.body().empty()) request_stream << request.body();

    // Send the request.
    boost::asio::write(socket, requestBuf, ec);
    if (ec) return HandleError("write", ec);

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf resp;
    boost::asio::read_until(socket, resp, "\r\n", ec);
    if (ec) return HandleError("read_until", ec);

    // Check that response is OK.
    std::istream response_stream(&resp);
    std::string http_version;
    response_stream >> http_version;

    unsigned int status_code;
    response_stream >> status_code;

    std::string status_message;
    std::getline(response_stream, status_message);
    
    response->SetVersion(http_version);
    response->SetStatus(status_code);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
      response->SetStatus(Response::internal_server_error);
      return RequestHandler::handle_fail;
    }
    if (status_code != 200 && status_code != 302) {
      return RequestHandler::handle_fail;
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, resp, "\r\n\r\n", ec);
    if (ec) return HandleError("read_until", ec);

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r"){
      header.pop_back();
	  std::string head = header.substr(0, header.find(":"));
	  std::string value = header.substr(header.find(":") + 2);

	  if(head == "Cache-Control" 
	     && value.find("no-store") == std::string::npos 
	     && value.find("no-cache") == std::string::npos) {
	    //Prevents the browser from doing its own caching
	    response->AddHeader(head, "no-store");

	    std::size_t location_of_max_age = value.find("max-age");
	    if(location_of_max_age != std::string::npos) {
	      std::string cache_life = value.substr(location_of_max_age);
	      std::size_t end_location = cache_life.find(",");
	      if(end_location != std::string::npos) {
		cache_life = cache_life.substr(0, end_location);
	      }
	    
	      cache_life = cache_life.substr(8);
	      boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	      boost::posix_time::time_duration cache_duration(0, 0, std::stoi(cache_life), 0);
	      boost::posix_time::ptime expiration = now + cache_duration;
	      cache_[request.uri()] = expiration;
	    }
	  }
	  else {
	    response->AddHeader(head, value);
	  }  
	  if(status_code == 302 && head.compare("Location")==0) {
		// absolute_path: Location: http(s)://host(:port)/url(?...)
		// relative_path: Location: /url(?...)
		ParseLocation(value, server_host, server_port, uri);
		break;
	  }
    }

    if (cache_.find(request.uri()) != cache_.end()) {
	is_cache = true;
    }
    if(is_cache) {
      cache_file.open("server_cache/"+request.uri());
    }

    // Read until EOF, writing data to output as we go.
    std::ostringstream ss;
  	ss << &resp;
  	response->SetBody(ss.str());
	if(is_cache)
	  cache_file << ss.str();

    while (boost::asio::read(socket, resp, ec)) {
      std::ostringstream ss;
      ss << &resp;
      response->AppendBody(ss.str());
      if(is_cache)
	cache_file << ss.str();
      if (ec) {
        break;
      }
    }
    
    if (ec != boost::asio::error::eof) {
      response->SetStatus(Response::internal_server_error);
      if(cache_file.is_open())
	cache_file.close();
      return RequestHandler::handle_fail;
    }

    if(status_code == 200)
      break;
  }
  if(cache_file.is_open())
    cache_file.close();
  return RequestHandler::ok;
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
