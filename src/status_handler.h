#ifndef HTTP_STATUS_REQUEST_HANDLER_H
#define HTTP_STATUS_REQUEST_HANDLER_H

#include <string>
#include "request_handler.h"
#include "server_status.h"

namespace http {
namespace server{
/// The common handler for all incoming requests.
class StatusHandler: public RequestHandler
{
 public:
  StatusHandler() {};

  RequestHandler::Status Init(const std::string& uri_prefix,
                              const NginxConfig& config);

  RequestHandler::Status HandleRequest(const Request& request,
                                       Response* response);
  
 private:
  std::string uri_prefix_;

};

REGISTER_REQUEST_HANDLER(StatusHandler);

} // namespace server
} // namespace http

#endif // HTTP_STATUS_REQUEST_HANDLER_H