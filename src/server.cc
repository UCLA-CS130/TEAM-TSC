
#include "server.h"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"
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
  //intialize how many thread to run on the server
  total_thread_num_ = server_opt.threadCount;


  // initialize echo_handlers
  // echo_config is an essential parameter in Init(). However EchoHandler doesn't have config_block
  NginxConfig config;
  for (auto uri_prefix: server_opt.echo_uri_prefixes) {
    handlers[uri_prefix] = std::unique_ptr<RequestHandler>(RequestHandler::CreateByName("EchoHandler"));
    RequestHandler::Status status = handlers[uri_prefix]->Init(uri_prefix, config);
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

  // initialize python_handlers
  unsigned int python_handler_num = server_opt.python_uri_prefixes.size();
  for (unsigned int i = 0; i < python_handler_num; ++i) {
    std::string uri_prefix = server_opt.python_uri_prefixes[i];
    NginxConfig python_config = server_opt.python_config[i];

    handlers[uri_prefix] = std::unique_ptr<RequestHandler>(RequestHandler::CreateByName("PythonHandler"));
    RequestHandler::Status status= handlers[uri_prefix]->Init(uri_prefix, python_config);
    if (status != RequestHandler::ok) {
      std::cerr << "Error: Initialization of PythonHandler\n";
      return false;
    }
    ServerStatus::getInstance().addHandlerToUri("PythonHandler",uri_prefix);
  }
  
  // initialize proxy_handlers
  unsigned int proxy_handler_num = server_opt.proxy_uri_prefixes.size();
  for (unsigned int i = 0; i < proxy_handler_num; ++i) {
    std::string uri_prefix = server_opt.proxy_uri_prefixes[i];
    NginxConfig proxy_config = server_opt.proxy_config[i];

    handlers[uri_prefix] = std::unique_ptr<RequestHandler>(RequestHandler::CreateByName("ProxyHandler"));
    RequestHandler::Status status= handlers[uri_prefix]->Init(uri_prefix, proxy_config);
    if (status != RequestHandler::ok) {
      std::cerr << "Error: Initialization of ProxyHandler\n";
      return false;
    }
    ServerStatus::getInstance().addHandlerToUri("ProxyHandler",uri_prefix);
  }
  
  NginxConfig error_config;
  handlers["ErrorHandler"] = std::unique_ptr<RequestHandler>(RequestHandler::CreateByName("ErrorHandler"));
  RequestHandler::Status err_handler_status = handlers["ErrorHandler"]->Init("ErrorHandler", error_config);
  if(err_handler_status != RequestHandler::ok) {
    std::cerr << "Error: Initialization of ErrorHandler\n";
    return false;
  }


  // initialize db_handlers
  handlers["/DbHandler"] = std::unique_ptr<RequestHandler>(RequestHandler::CreateByName("DbHandler"));
  handlers["/DbHandler"]->Init("DbHandler", config);

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
  std::vector<boost::shared_ptr<boost::thread>> thread_pool;
  for(int index = 0; index < total_thread_num_; index++){
    boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
    thread_pool.push_back(thread);

  }
  for(int index = 0; index < total_thread_num_ ; index++){
    thread_pool[index] -> join();
  }
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
