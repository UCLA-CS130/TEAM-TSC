
#include <string>
#include "echo_request_handler.h"

namespace http {
namespace server {

EchoRequestHandler::EchoRequestHandler(const std::vector<std::string>& serve_paths_): 
    RequestHandler(serve_paths_) 
{
}

void
EchoRequestHandler::handle_request(const std::string req, reply& rep) {
    // Fill out the reply to be sent to the client.
    rep.status = reply::ok;
    rep.content.append(req);

    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = "text/plain";
}

} // namespace server
} // namespace http