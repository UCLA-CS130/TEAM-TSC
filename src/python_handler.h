#ifndef HTTP_PYTHON_HANDLER_H
#define HTTP_PYTHON_HANDLER_H

#include <string>
#include "request_handler.h"
#include "server_status.h"

namespace http {
namespace server {
  
/// The common handler for all incoming requests.
class PythonHandler: public RequestHandler
{
 public:
  PythonHandler() {};

  RequestHandler::Status Init(const std::string& uri_prefix_,
                              const NginxConfig& config);

  RequestHandler::Status HandleRequest(const Request& request,
                                       Response* response);

  using DecodedBody = std::vector<std::pair<std::string, std::string>>;

  bool ParseBody(const std::string& body, DecodedBody& decoded_body);

 private:
  std::string uri_prefix_;

  std::string base_dir;

  std::string entry_func;

};

REGISTER_REQUEST_HANDLER(PythonHandler);

} // server
} // http

#endif // HTTP_PYTHON_HANDLER_H