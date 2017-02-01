
#include "server.h"
#include <utility>

namespace http {
namespace server {

Server::Server(const config_opts& server_config)
	: io_service_(),
	  acceptor_(io_service_),
	  socket_(io_service_)
{
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({server_config.address, server_config.port});
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  do_accept();
}

Server::Server()
  : io_service_(),
    acceptor_(io_service_),
    socket_(io_service_)
{
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({"0.0.0.0", "8080"});
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  do_accept();
}

void Server::run()
{
  io_service_.run();
}

void Server::do_accept()
{
  acceptor_.async_accept(socket_,
      [this](boost::system::error_code ec)
      {
        if (!ec) {
          std::make_shared<Connection>(std::move(socket_))->start();
        }

        do_accept();
      });
}

} //server
} //http
