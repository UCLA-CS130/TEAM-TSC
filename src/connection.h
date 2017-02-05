#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include <boost/asio.hpp>
#include <string>
#include <boost/bind.hpp>
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "request_parser.h"

namespace http {
namespace server {

class Connection
	: public std::enable_shared_from_this<Connection>
{
public:
	  Connection(const Connection&) = delete;
  	Connection& operator=(const Connection&) = delete;

  	explicit Connection(boost::asio::ip::tcp::socket socket, 
                        EchoRequestHandler& echo_request_handler_, 
                        StaticRequestHandler&  static_request_handler_);

  	void start();

  	bool handle_read(const boost::system::error_code& ec, 
                     std::size_t bytes_transferred);

  	bool handle_write(const boost::system::error_code& ec,
  					  std::size_t);

private:
	void do_read();

	void do_write();

	/// The handler used to process the incoming request.
  EchoRequestHandler& echo_request_handler;

  StaticRequestHandler& static_request_handler;

  RequestParser request_parser;
  
	boost::asio::ip::tcp::socket socket_;

	std::array<char, 8192> buffer_;

	std::string request_string;

  reply reply_;

  request request_;


};

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP