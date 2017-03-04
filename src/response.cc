#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>
#include "response.h"

namespace http{
namespace server{

const std::string ok =
  "HTTP/1.0 200 OK\r\n";
const std::string bad_request =
  "HTTP/1.0 400 Bad Request\r\n";
const std::string not_found =
  "HTTP/1.0 404 Not Found\r\n";
const std::string internal_server_error =
  "HTTP/1.0 500 Internal Server Error\r\n";
const std::string redirect =
  "HTTP/1.0 302 Redirect\r\n";

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

std::string
Response::ToString()
{
  std::string str = "";
  str += status_to_string(status_);
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