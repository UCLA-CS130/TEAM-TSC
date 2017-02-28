#ifndef HTTP_PROXY_HANDLER_H
#define HTTP_PROXY_HANDLER_H

#include <string>
#include "request_handler.h"
#include "server_status.h"

namespace http {
namespace server {
  
/// The common handler for all incoming requests.
class ProxyHandler: public RequestHandler
{
 public:
  ProxyHandler() {};

  RequestHandler::Status Init(const std::string& uri_prefix_,
                              const NginxConfig& config);

  RequestHandler::Status HandleRequest(const Request& request,
                                       Response* response);

 private:
  std::string uri_prefix;

  std::string url;
  std::string portno;
};

REGISTER_REQUEST_HANDLER(ProxyHandler);

} // server
} // http

#endif // HTTP_PROXY_HANDLER_H
