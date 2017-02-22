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

  RequestHandler::Status ErrorHandler::HandleRequest(const Request& request,
						      Response* response)
  {
    if(response->IsStatus(Response::not_found))
      {
	std::string err_message ="<html>\n\n<head>\n\t<title>404 Not Found</title>\n</head>\n\n<body>\n\t<h1>Not Found</h1>\n\t<p>The requested URL was not found on this server.</p>\n</body>\n\n</html>";
	response->SetBody(err_message);
	response->AddHeader("Content-Length", std::to_string(err_message.size()));
	response->AddHeader("Content-Type", "text/html");
	return RequestHandler::ok;
			    
      }
    else if (response->IsStatus(Response::bad_request))
      {
	std::string err_message ="<html>\n\n<head>\n\t<title>400 Bad Request</title>\n</head>\n\n<body>\n\t<h1>Not Found</h1>\n\t<p>Invalid Request</p>\n</body>\n\n</html>";
        response->SetBody(err_message);
        response->AddHeader("Content-Length", std::to_string(err_message.size()));
        response->AddHeader("Content-Type", "text/html");
        return RequestHandler::ok;

      }
    return RequestHandler::handle_fail;
  }

} //namespace server
} //namespace http
