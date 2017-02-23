
#include "server.h"
#include <utility>

namespace http {
namespace server {




Server::Server():
    io_service_(),
	  acceptor_(io_service_),
	  socket_(io_service_)
{
}

bool 
Server::init(const char* config_file_path)
{
  NginxConfigParser config_parser;
  ConfigHandler config_handler(config_parser);
  if (!config_handler.handle_config(config_file_path)) {
    std::cerr << "Error: illegal config format\n";
    return false;
  }
  config_opts server_opt = config_handler.get_config_opt();


  // initialize echo_handlers
  // echo_config is an essential parameter in Init(). However EchoHandler doesn't have config_block
  NginxConfig echo_config;
  for (auto uri_prefix: server_opt.echo_uri_prefixes) {
    handlers[uri_prefix] = std::unique_ptr<RequestHandler>(RequestHandler::CreateByName("EchoHandler"));
    RequestHandler::Status status = handlers[uri_prefix]->Init(uri_prefix, echo_config);
    if (status != RequestHandler::ok) {
      std::cerr << "Error: Initialization of EchoHandler\n";
      return false;
    }
    ServerStatus::getInstance().addHandlerToUri("EchoHandler",uri_prefix);
  }

  //intialize status_handlers
  NginxConfig status_config;
  for (auto uri_prefix: server_opt.status_uri_prefixes) {
    handlers[uri_prefix] = std::unique_ptr<RequestHandler>(RequestHandler::CreateByName("StatusHandler"));
    RequestHandler::Status status = handlers[uri_prefix]->Init(uri_prefix,status_config);
    if(status != RequestHandler::ok){
      std::cerr << "Error: Initialization of StatusHandler\n";
      return false;
    }
    ServerStatus::getInstance().addHandlerToUri("StatusHandler",uri_prefix);
  }

  // initialize static_handlers
  // BOOST_ASSERT(server_opt.static_file_uri_prefixes.size() == server_opt.static_file_config.size())
  unsigned int static_file_handler_num = server_opt.static_file_uri_prefixes.size();
  for (unsigned int i = 0; i < static_file_handler_num; ++i) {
    std::string uri_prefix = server_opt.static_file_uri_prefixes[i];
    NginxConfig static_config = server_opt.static_file_config[i];

    handlers[uri_prefix] = std::unique_ptr<RequestHandler>(RequestHandler::CreateByName("StaticFileHandler"));
    RequestHandler::Status status= handlers[uri_prefix]->Init(uri_prefix, static_config);
    if (status != RequestHandler::ok) {
      std::cerr << "Error: Initialization of StaticHandler\n";
      return false;
    }
    ServerStatus::getInstance().addHandlerToUri("StaticHandler",uri_prefix);
  }

  NginxConfig error_config;
  handlers["ErrorHandler"] = std::unique_ptr<RequestHandler>(RequestHandler::CreateByName("ErrorHandler"));
  RequestHandler::Status err_handler_status = handlers["ErrorHandler"]->Init("ErrorHandler", error_config);
  if(err_handler_status != RequestHandler::ok) {
    std::cerr << "Error: Initialization of ErrorHandler\n";
    return false;
  }


  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({server_opt.address, server_opt.port});
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  do_accept();
  return true;
}

void Server::run()
{
  io_service_.run();
}

void Server::do_accept()
{
  acceptor_.async_accept(socket_,
      boost::bind(&Server::handle_accept, this,
                  boost::asio::placeholders::error));
}

// for gtest (bool make test convenient)
bool Server::handle_accept(const boost::system::error_code& ec) {
  if (!ec) {
    std::make_shared<Connection>(std::move(socket_), 
                                 handlers)->start();
  }
  else return false;
  do_accept();
  return true;
}

} //server
} //http
