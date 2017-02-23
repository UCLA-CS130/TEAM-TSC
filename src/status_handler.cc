#include "status_handler.h"

namespace http {
namespace server {

RequestHandler::Status
StatusHandler::Init(const std::string& uri_prefix_, const NginxConfig& config)
{
	uri_prefix = uri_prefix_;
	return RequestHandler::ok;
}

RequestHandler::Status
StatusHandler::HandleRequest(const Request& request, Response* response)
{
    // Fill out the reply to be sent to the client.
    response->SetStatus(Response::ok);
    std::string body = ServerStatus::getInstance().getStatusString();
    response->SetBody(body);
    response->AddHeader("Content-Length",  std::to_string(body.size()));
    response->AddHeader("Content-Type", "text/plain");
    ServerStatus::getInstance().addUri("/status");
    return RequestHandler::ok;
}


} // namespace server
} // namespace http