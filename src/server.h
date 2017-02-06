#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <boost/asio.hpp>
#include <string>
#include "connection.h"
#include "config_opts.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "request_parser.h"

namespace http {
namespace server {

class Server {
public:
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  explicit Server(const config_opts& server_config);

  void run();

  bool handle_accept(RequestParserInterface *request_parser, 
                     const boost::system::error_code& ec);

private:
  boost::asio::io_service io_service_;

  boost::asio::ip::tcp::acceptor acceptor_;

  boost::asio::ip::tcp::socket socket_;

  EchoRequestHandler echo_request_handler;
  
  StaticRequestHandler static_request_handler;

  void do_accept();
};


} // namespace server
} // namespace server

#endif // HTTP_SERVER_HPP
