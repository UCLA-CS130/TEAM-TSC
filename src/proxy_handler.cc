#include "proxy_handler.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>

namespace http{
	namespace server{
		RequestHandler::Status ProxyHandler::Init(const std::string& uri_prefix_, const NginxConfig& config){
			uri_prefix = uri_prefix_;
  			//int root_num = 0;
  			for (auto statement: config.statements_) {
    			std::string start_token = statement->tokens_[0];
    			if (start_token == "host") {
      				if(statement->tokens_.size() != 1){
      					host_name = statement->tokens_[1];
      				}
      				else {
      					//throw error.
      				}
    			}

    			if(start_token == "port"){
    				if(statement->tokens_.size() != 1){
      					portno = statement->tokens_[1];
      				}
      				else {
      					//throw error.
      				}
    			}
    		// other statements
  			}
  			return RequestHandler::ok;
		}

		using boost::asio::ip::tcp;

  		RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response){
  			boost::asio::io_service io_service;
		    tcp::resolver resolver(io_service);
		    tcp::resolver::query query(host_name, "http");
		    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		    tcp::socket socket(io_service);
		    boost::asio::connect(socket, endpoint_iterator);
		    
		    std::string uri = request.uri() == uri_prefix ? "/" : request.uri();

		    boost::asio::streambuf requestBuf;
		    std::ostream request_stream(&requestBuf);
		    request_stream << "GET " << uri << " HTTP/1.1\r\n";
		    request_stream << "Host: " << host_name << ":" << portno << "\r\n"; 
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
		    else if(status_code == 302){
		      response->SetStatus(Response::redirect);
		      //return RequestHandler::handle_fail;
		    }
		    else if(status_code == 404){
		      response->SetStatus(Response::not_found);
		      return RequestHandler::handle_fail;
		    }
		    else {
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
		    }

		    // Read until EOF, writing data to output as we go.
		    boost::system::error_code error;
		    while (boost::asio::read(socket, resp, error)) {
		        std::ostringstream ss;
		        ss << &resp;
		        response->SetBody(ss.str());
		        if (error == boost::asio::error::eof) {
		            break;
		        }
		    }
		    
		    if (error != boost::asio::error::eof)
		        std::cout << "Error: " << error << std::endl;

		    return RequestHandler::ok;
  		}
	}
}	
