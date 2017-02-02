#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include "connection.h"
#include "config_opts.h"

namespace http {
namespace server {

class Server {
public:
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  explicit Server(const config_opts& server_config);

  void run();

  bool handle_accept(const boost::system::error_code& ec);

private:

  void do_accept();

  boost::asio::io_service io_service_;

  boost::asio::ip::tcp::acceptor acceptor_;

  boost::asio::ip::tcp::socket socket_;
};


} // namespace server
} // namespace server

#endif // HTTP_SERVER_HPP
