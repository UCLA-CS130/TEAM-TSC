#include <string>
#include "echo_handler.h"

namespace http {
namespace server {

RequestHandler::Status
EchoHandler::Init(const std::string& uri_prefix_, const NginxConfig& config)
{
	uri_prefix = uri_prefix_;
	return RequestHandler::ok;
}

RequestHandler::Status
EchoHandler::HandleRequest(const Request& request, Response* response)
{
    // Fill out the reply to be sent to the client.
    response->SetStatus(Response::ok);
    response->SetBody(request.raw_request());
    response->AddHeader("Content-Length", std::to_string(request.raw_request().size()));
    response->AddHeader("Content-Type", "text/plain");
    return RequestHandler::ok;
}

} // namespace server
} // namespace http