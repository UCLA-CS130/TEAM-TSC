#include "proxy_handler.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>

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
		  if (!std::isdigit(portno[i])){
		    return RequestHandler::invalid_config;
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
  while(true) 
  {
  	boost::system::error_code ec;
	tcp::resolver resolver(io_service_);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve({server_host, server_port}, ec);
    

    tcp::socket socket(io_service_);
    boost::asio::connect(socket, endpoint_iterator);
    
    std::string uri = request.uri() == uri_prefix_ ? "/" : request.uri();

    //int loc = uri.substr(1).find_first_of("/");
	
	int findLoc = uri.find(uri_prefix_);

    if(findLoc != -1){
    	uri.erase(findLoc, uri_prefix_.size());
    }

    if (uri == "" || uri[0] != '/') uri = "/" + uri;

    std::cout << "URIBEINGSENT:  " << uri << std::endl;
	    boost::asio::streambuf requestBuf;
    std::ostream request_stream(&requestBuf);
    request_stream << "GET " << uri << " HTTP/1.1\r\n";
    request_stream << "Host: " << server_host << ":" << server_port << "\r\n"; 
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    boost::asio::write(socket, requestBuf);

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf resp;
    boost::asio::read_until(socket, resp, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&resp);
    std::string http_version;
    response_stream >> http_version;
    std::cout << "http version: " << http_version << std::endl;

    unsigned int status_code;
    response_stream >> status_code;
    std::cout << "status code: " << status_code << std::endl;

    std::string status_message;
    std::getline(response_stream, status_message);
    std::cout << "status message: " << status_message << std::endl;
    
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
      std::cout << "Invalid response\n";
      //return 1;
    }
    if (status_code == 200)
    {
      std::cout << "Response returned with status code " << status_code << "\n";
      response->SetStatus(Response::ok);
      //return 1;
    }
    else if(status_code == 404){
      response->SetStatus(Response::not_found);
      return RequestHandler::handle_fail;
    }
    else if(status_code == 400){
      response->SetStatus(Response::bad_request);
      return RequestHandler::handle_fail;
    }


    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, resp, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r"){
        std::cout << "response header: " << header << "\n";
		std::string head = header.substr(0, header.find(":"));
		std::string value = header.substr(header.find(":")+2);
		std::cout << head << "," << value << std::endl;
		response->AddHeader(head, value);
		if(status_code==302 && head.compare("Location")==0) {
			server_host = value;
			server_host = server_host.substr(server_host.find(":") +3);
			server_host = server_host.substr(0, server_host.length() - 2);
		}
    }

    // Read until EOF, writing data to output as we go.
    std::ostringstream ss;
  	ss << &response_stream_buf;
  	response->SetBody(ss.str());

    boost::system::error_code error;
    while (boost::asio::read(socket, resp, error)) {
        std::ostringstream ss;
        ss << &resp;
        response->AppendBody(ss.str());
        if (error) {
            break;
        }
    }
    
    if (error != boost::asio::error::eof)
        std::cout << "Error: " << error << std::endl;

    if(status_code != 302 && status_code != 301)
    	break;
  }
  return RequestHandler::ok;
}

} // server
} // http
