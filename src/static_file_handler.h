#ifndef HTTP_STATIC_REQUEST_HANDLER_H
#define HTTP_STATIC_REQUEST_HANDLER_H

#include <string>
#include "request_handler.h"

namespace http {
namespace server {
  
/// The common handler for all incoming requests.
class StaticFileHandler: public RequestHandler
{
 public:
  StaticFileHandler() {};

  RequestHandler::Status Init(const std::string& uri_prefix_,
                              const NginxConfig& config);

  RequestHandler::Status HandleRequest(const Request& request,
                                       Response* response);


 private:
  std::string uri_prefix;

  std::string base_dir;

  std::string extension2type(std::string);
};

REGISTER_REQUEST_HANDLER(StaticFileHandler);

} // server
} // http

#endif // HTTP_STATIC_REQUEST_HANDLER_H