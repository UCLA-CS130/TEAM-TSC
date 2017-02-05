#ifndef HTTP_ECHO_REQUEST_HANDLER_H
#define HTTP_ECHO_REQUEST_HANDLER_H

#include <string>
#include "request_handler.h"
#include <vector>

namespace http {
namespace server {

/// The common handler for all incoming requests.
class EchoRequestHandler: public RequestHandler
{
 public:
  EchoRequestHandler(const std::vector<std::string>& valid_paths_);
  
  void handle_request(const std::string req, reply& rep);
};

} // server
} // http

#endif // HTTP_ECHO_REQUEST_HANDLER_H
