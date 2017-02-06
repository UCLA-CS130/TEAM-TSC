#ifndef HTTP_REQUEST_HANDLER_H
#define HTTP_REQUEST_HANDLER_H

#include <string>
#include "request.h"
#include "reply.h"
#include <vector>

namespace http {
namespace server {

/// The common handler for all incoming requests.
class RequestHandler
{
public:
  virtual ~RequestHandler() {};
  /// Handle a request and produce a reply.
  virtual void handle_request(const std::string req_str, const request& req, reply& rep) = 0;

  //virtual void handle_request(const std::string req, reply& rep) = 0;

  virtual bool check_serve_path(std::string uri) = 0;

};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP