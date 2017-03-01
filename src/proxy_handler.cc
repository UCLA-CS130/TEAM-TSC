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
  			try
  			{
			    boost::asio::io_service io_service;

			    // Get a list of endpoints corresponding to the server name.
			    tcp::resolver resolver(io_service);
			    tcp::resolver::query query(host_name, "http");
			    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

			    // Try each endpoint until we successfully establish a connection.
			    tcp::socket socket(io_service);
			    boost::asio::connect(socket, endpoint_iterator);

			    // Form the request. We specify the "Connection: close" header so that the
			    // server will close the socket after transmitting the response. This will
			    // allow us to treat all data up until the EOF as the content.
			    // boost::asio::streambuf request;
			    // std::ostream request_stream(&request);
			    // request_stream << request.raw_request();

			    // Modify the original request.
			    std::string req = request.raw_request();
			    std::cout << "old raw request: " << req << std::endl;
			    
			    // remove the proxy's uri_prefix from the uri in the new request
			    size_t index = req.find(uri_prefix);
			    if (index == std::string::npos)
			      {
				std::cout << "Error: proxy uri-prefix not found in original request to proxy handler" << std::endl;
				response->SetStatus(Response::bad_request);
				return RequestHandler::handle_fail;
			      }
			    // erase the next size(proxy-uri-prefix) chars starting at index
			    req.erase(index, uri_prefix.size());
			    
			    // replace the old host header with a new host header
			    // TODO: add some error handling
			    size_t start = req.find("Host");
			    // look for the next '\r' after "Host" to find the end of the host header field
			    size_t end = req.find('\r', start);
			    std::string host_field = req.substr(start, end-start);
			    std::cout << "host field: " << host_field << std::endl;

			    std::string new_host_field = "Host: " + host_name + ":" + portno;
			    std::cout << "new host field: " << new_host_field << std::endl;
			    
			    // replace the old host field with the new host field
			    req.replace(start, end-start, new_host_field);

			    std::cout << "new raw request: " << req << std::endl;

			    boost::asio::write(socket, boost::asio::buffer(req, req.size()));

			    // Read the response status line. The response streambuf will automatically
			    // grow to accommodate the entire line. The growth may be limited by passing
			    // a maximum size to the streambuf constructor.
			    boost::asio::streambuf responseBuf;
			    boost::asio::read_until(socket, responseBuf, "\r\n");

			    // Check that response is OK.
			    std::istream response_stream(&responseBuf);
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
			    else {
			      response->SetStatus(Response::bad_request);
			      return RequestHandler::handle_fail;
			    }

			    // Read the response headers, which are terminated by a blank line.
			    boost::asio::read_until(socket, responseBuf, "\r\n\r\n");

			    // Process the response headers.
			    std::string header;
			    while (std::getline(response_stream, header) && header != "\r"){
			        std::cout << "response header: " << header << "\n";
			      	boost::char_separator<char> separator{": "};
    				boost::tokenizer<boost::char_separator<char>> tokens(header, separator);
					boost::tokenizer<boost::char_separator<char>>::iterator tokens_it = tokens.begin();
					response->AddHeader(*(tokens_it), *(++tokens_it));
			    }
			    std::cout << "\n";
			    
			    // std::ostringstream ss;
			    // if(responseBuf.size() > 0)
			    // 	ss << &responseBuf;
			    //std::string respBody = &responseBuf;
			    // Write whatever content we already have to output.
			    //if (responseBuf.size() > 0)
			      //std::cout << &responseBuf;
			   	//std::ostringstream ss;
			   //  Read until EOF, writing data to output as we go.
			    boost::system::error_code error;
			    while (boost::asio::read(socket, responseBuf,
			          boost::asio::transfer_at_least(1), error)){
			      //std::cout << &responseBuf;
			    	//respBody.append(&responseBuf);
			    	//ss << &responseBuf;
			    }
			    if (error != boost::asio::error::eof)
			      throw boost::system::system_error(error);
		  		// std::string respBody;
		  		// respBody = ss.str();
		  		// std::cout << respBody << std::endl;
		  		// response->SetBody(respBody);
		  	}
		  	catch (std::exception& e)
		  	{
		    	std::cout << "Exception: " << e.what() << "\n";
		  	}

  			return RequestHandler::ok;
  		}
	}
}	
