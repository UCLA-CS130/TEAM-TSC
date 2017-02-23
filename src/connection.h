#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include <boost/asio.hpp>
#include <string>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include "request_handler.h"
#include "request.h"
#include "response.h"

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

  	bool handle_read(const boost::system::error_code& ec, 
                     std::size_t bytes_transferred);

  	bool handle_write(const boost::system::error_code& ec,
  					          std::size_t);

    bool ProcessRequest(const std::string& uri_prefix);

private:
  boost::asio::ip::tcp::socket socket_;
	
  std::map<std::string, std::unique_ptr<RequestHandler>>& handlers;

	std::array<char, 8192> buffer_;

  Response response;

  Request request;

  void do_read();

  void do_write();
};

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP