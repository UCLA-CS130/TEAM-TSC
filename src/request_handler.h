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
  RequestHandler(const std::vector<std::string>& serve_paths_): serve_paths(serve_paths_) {};

  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep);

  void handle_request(const std::string req, reply& rep);

  bool check_serve_path(std::string uri) {
    for (auto i: serve_paths) {
      std::size_t found = uri.find(i);
      if (found == 0) {
        return true;
      }
    }
    return false;
  }

private:
  std::vector<std::string> serve_paths;

};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP