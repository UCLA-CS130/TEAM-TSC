#include <string>
#include "echo_request_handler.h"

namespace http {
namespace server {

EchoRequestHandler::EchoRequestHandler(const handler_opts& handler_opts_):
  serve_paths(handler_opts_.paths) 
{
}

bool
EchoRequestHandler::handle_request(const std::string req_str, const request& req, reply& rep) {
    // Fill out the reply to be sent to the client.
    rep.status = reply::ok;
    rep.content.append(req_str);

    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = "text/plain";
    return true;
}

bool 
EchoRequestHandler::check_serve_path(std::string uri) {
  for (auto i: serve_paths) {
    std::size_t found = uri.find(i);
    if (found == 0) {
      return true;
    }
  }
  return false;
}

} // namespace server
} // namespace http