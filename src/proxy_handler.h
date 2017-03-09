#ifndef HTTP_PROXY_HANDLER_H
#define HTTP_PROXY_HANDLER_H

#include <string>
#include <boost/asio.hpp>
#include "request_handler.h"
#include "server_status.h"
#include "request.h"

namespace http {
namespace server {
  
/// The common handler for all incoming requests.
class ProxyHandler: public RequestHandler
{
 public:
  ProxyHandler();

  RequestHandler::Status Init(const std::string& uri_prefix,
                              const NginxConfig& config);

  RequestHandler::Status HandleRequest(const Request& request,
                                       Response* response);

  RequestHandler::Status HandleError(const std::string& error_info, 
                                     const boost::system::error_code& ec);

  void ParseLocation(const std::string& location_header,
                     std::string& server,
                     std::string& port,
                     std::string& path);

 private:
  boost::asio::io_service io_service_;

  std::string uri_prefix_;

  std::string server_host_;

  std::string server_port_;
};

REGISTER_REQUEST_HANDLER(ProxyHandler);

} // server
} // http

#endif // HTTP_PROXY_HANDLER_H
