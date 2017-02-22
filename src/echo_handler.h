#ifndef HTTP_ECHO_REQUEST_HANDLER_H
#define HTTP_ECHO_REQUEST_HANDLER_H

#include <string>
#include "request_handler.h"
#include "server_status.h"

namespace http {
namespace server {

/// The common handler for all incoming requests.
class EchoHandler: public RequestHandler
{
 public:
  EchoHandler() {};

  RequestHandler::Status Init(const std::string& uri_prefix_,
                              const NginxConfig& config);

  RequestHandler::Status HandleRequest(const Request& request,
                                       Response* response);
  

 private:
  std::string uri_prefix;

};

REGISTER_REQUEST_HANDLER(EchoHandler);

} // server
} // http

#endif // HTTP_ECHO_REQUEST_HANDLER_H
