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
  EchoRequestHandler(const std::vector<std::string>& serve_paths_);
  
  //void handle_request(const request& req, reply& rep) {};

  //void handle_request(const std::string req, reply& rep);
  bool handle_request(const std::string req_str, const request& req, reply& rep);

  bool check_serve_path(std::string uri);

 private:
  std::vector<std::string> serve_paths;

};

} // server
} // http

#endif // HTTP_ECHO_REQUEST_HANDLER_H
