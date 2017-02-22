#include <string>
#include "error_handler.h"

namespace http {
namespace server {

RequestHandler::Status ErrorHandler::Init(const std::string& uri_prefix_,
				                                  const NginxConfig& config)
{
  uri_prefix = uri_prefix_;
  return RequestHandler::ok;
}

RequestHandler::Status 
ErrorHandler::HandleRequest(const Request& request, Response* response)
{
  std::string err_message = to_string(response->GetStatus());
  response->SetBody(err_message);
  response->AddHeader("Content-Length", std::to_string(err_message.size()));
  response->AddHeader("Content-Type", "text/html");
  return RequestHandler::ok; 
}

const char bad_request[] =
  "<html>"
  "<head><title>Bad Request</title></head>"
  "<body><h1>400 Bad Request</h1></body>"
  "</html>";
const char not_found[] =
  "<html>"
  "<head><title>Not Found</title></head>"
  "<body><h1>404 Not Found</h1><img src=\"pooh_not_found.png\"></body>"
  "</html>";
const char internal_server_error[] =
  "<html>"
  "<head><title>Internal Server Error</title></head>"
  "<body><h1>500 Internal Server Error</h1></body>"
  "</html>";

std::string 
ErrorHandler::to_string(Response::ResponseCode status)
{
  switch (status) 
  {
  case Response::not_found:
    return not_found;
  case Response::bad_request:
    return bad_request;
  default:
    return internal_server_error;
  }
}

} //namespace server
} //namespace http
