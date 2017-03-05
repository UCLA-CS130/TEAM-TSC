#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>
#include "response.h"

namespace http{
namespace server{

const std::string ok =
  "200 OK\r\n";
const std::string bad_request =
  "400 Bad Request\r\n";
const std::string not_found =
  "404 Not Found\r\n";
const std::string internal_server_error =
  "500 Internal Server Error\r\n";
const std::string redirect =
  "302 Redirect\r\n";

static inline std::string 
status_to_string(Response::ResponseCode code)
{
  switch (code)
  {
  case Response::ok:
    return ok;
  case Response::bad_request:
    return bad_request;
  case Response::not_found:
    return not_found;
  case Response::internal_server_error:
    return internal_server_error;
  case Response::redirect:
    return redirect;
  default:
    return internal_server_error;
  }
}

void
Response::SetStatus(const unsigned int response_code)
{
  if (response_code == 200)
    status_ = Response::ok;
  else if (response_code == 400)
    status_ = Response::bad_request;
  else if (response_code == 404)
    status_ = Response::not_found;
  else if (response_code == 500)
    status_ = Response::internal_server_error;
  else if (response_code == 302)
    status_ = Response::redirect;
  else 
    status_ = Response::internal_server_error;
}

std::string
Response::ToString()
{
  std::string str = "";
  if (version_ == "") version_ = "HTTP/1.1"; 
  str += version_ + " " + status_to_string(status_);
  for (auto header: headers_) {
  	str += header.first;
  	str += ": ";
  	str += header.second;
  	str += "\r\n";
  }
  str += "\r\n";
  str += body_;
  return str;
}

} // namespace server
} // namespace http