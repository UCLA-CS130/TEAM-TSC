#include <string>
#include "echo_handler.h"

namespace http {
namespace server {

RequestHandler::Status
EchoHandler::Init(const std::string& uri_prefix, const NginxConfig& config)
{
	uri_prefix_ = uri_prefix;
	return RequestHandler::ok;
}

RequestHandler::Status
EchoHandler::HandleRequest(const Request& request, Response* response)
{
    // Fill out the reply to be sent to the client.
    std::string raw_request = request.raw_request();
    response->SetStatus(Response::ok);
    response->SetBody(raw_request);
    response->AddHeader("Content-Length", std::to_string(raw_request.size()));
    response->AddHeader("Content-Type", "text/plain");
    return RequestHandler::ok;
}

} // namespace server
} // namespace http
