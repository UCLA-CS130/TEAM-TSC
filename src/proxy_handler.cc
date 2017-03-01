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
    			if (start_token == "url") {
      				if(statement->tokens_.size() != 1){
      					url = statement->tokens_[1];
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
			    tcp::resolver::query query(url, "http");
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

			    // Send the request.
			    std::string req = request.raw_request();
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
			    unsigned int status_code;
			    response_stream >> status_code;
			    std::string status_message;
			    std::getline(response_stream, status_message);
			    //std::cout << "Got here baby." << std::endl;
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
			      	std::cout << header << "\n";
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
			    if (responseBuf.size() > 0)
			      std::cout << &responseBuf;
			   	//std::ostringstream ss;
			   //  Read until EOF, writing data to output as we go.
			    boost::system::error_code error;
			    while (boost::asio::read(socket, responseBuf,
			          boost::asio::transfer_at_least(1), error)){
			        std::cout << &responseBuf;
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
