#ifndef HTTP_ERROR_REQUEST_HANDLER_H
#define HTTP_ERROR_REQUEST_HANDLER_H

#include <string>
#include "request_handler.h"
#include "server_status.h"

namespace http {
namespace server {

class ErrorHandler : public RequestHandler
{
 public:
  ErrorHandler() {};
  
  RequestHandler::Status Init(const std::string& uri_prefix,
							  const NginxConfig& config);

  RequestHandler::Status HandleRequest(const Request& request,
				 					   Response* response);

  std::string to_string(Response::ResponseCode status);

};

REGISTER_REQUEST_HANDLER(ErrorHandler);

}
}

#endif // HTTP_ERROR_REQUEST_HANDLER_H
