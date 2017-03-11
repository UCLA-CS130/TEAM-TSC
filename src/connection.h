#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include "request_handler.h"
#include "server_status.h"
#include "request.h"
#include "response.h"
#include "body_compression.h"

namespace http {
namespace server {

class Connection
	: public std::enable_shared_from_this<Connection>
{
public:
	  Connection(const Connection&) = delete;
  	Connection& operator=(const Connection&) = delete;

  	explicit Connection(boost::asio::ip::tcp::socket socket, 
                        std::map<std::string, std::unique_ptr<RequestHandler>>& handlers_);

  	void start();

  	bool handle_read_partial(const boost::system::error_code& ec, 
                             std::size_t bytes_transferred);

    bool handle_read_body(const boost::system::error_code& ec, 
                          size_t bytes_transferred); 

    bool ProcessRequest(const Request& request);

    void compress_payload();

  	bool handle_write(const boost::system::error_code& ec,
  					          std::size_t);


private:
  boost::asio::ip::tcp::socket socket_;
	
  std::map<std::string, std::unique_ptr<RequestHandler>>& handlers;
  //change to streambuf to fit in async_read_until function
  boost::asio::streambuf buffer_;

  Response response;

  Request request;

  std::string raw_request;

  void do_read_partial();

  void do_read_body(std::size_t left_content_length);

  void do_write();
};

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP