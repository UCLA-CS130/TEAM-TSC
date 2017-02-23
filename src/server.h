#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <boost/asio.hpp>
#include <string>
#include <map>
#include "connection.h"
#include "config_handler.h"
#include "request_handler.h"
#include "server_status.h"

namespace http {
namespace server {


class Server {
public:
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  Server();

  bool init(const char* config_file_path);

  void run();

  bool handle_accept(const boost::system::error_code& ec);

private:
  boost::asio::io_service io_service_;

  boost::asio::ip::tcp::acceptor acceptor_;

  boost::asio::ip::tcp::socket socket_;

  std::map<std::string, std::unique_ptr<RequestHandler>> handlers;

  void do_accept();
};


} // namespace server
} // namespace server

#endif // HTTP_SERVER_HPP
