#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>
#include "request_handler.h"

namespace http{
namespace server{

std::unique_ptr<Request>
Request::Parse(const std::string& raw_request) 
{
  // make_unique is not the member of c++11
  std::unique_ptr<Request> req = std::unique_ptr<Request>(new Request());
  req->SetRawRequest(raw_request);

  // split the request_line, headers and body
  boost::cmatch request_mat;
  boost::regex request_expression ("([^\r\n]*)\r\n(.*)\r\n\r\n(.*)");
  if (!boost::regex_match(raw_request.c_str(), request_mat, request_expression)) {
    BOOST_LOG_TRIVIAL(info) << "Invalid request format\n";
    return nullptr;
  }
  std::string request_line = std::string(request_mat[1]);
  std::string headers_str = std::string(request_mat[2]);
  std::string body_str = std::string(request_mat[3]);

  // parse the request_line
  boost::cmatch request_line_mat;
  boost::regex request_line_expression ("(\\S+)\\s(/\\S*)\\s(HTTP/\\d+\\.\\d+)");
  if (!boost::regex_match(request_line.c_str(), request_line_mat, request_line_expression)) {
  	BOOST_LOG_TRIVIAL(info) << "Invalid request_line format\n";
  	return nullptr;
  }
  req->SetMethod(std::string(request_line_mat[1]));
  req->SetUri(std::string(request_line_mat[2]));
  req->SetVersion(std::string(request_line_mat[3]));
  if (req->uri().find("..") != std::string::npos) {
    BOOST_LOG_TRIVIAL(info) << "Invalid request_line format\n";
    return nullptr;
  }

  // parse the headers
  std::size_t pos = 0;
  while(true) {
  	std::size_t header_end = headers_str.find("\r\n", pos);
  	std::string header_str = headers_str.substr(pos, header_end);

    boost::cmatch header_mat;
  	boost::regex header_expression ("(\\S+)\\s*:\\s*(\\S.*)");
  	if (!boost::regex_match(header_str.c_str(), header_mat, header_expression)) {
  	  BOOST_LOG_TRIVIAL(info) << "Invalid header format\n";
  	  return nullptr;
  	}
    req->AddHeader(std::string(header_mat[1]), std::string(header_mat[2]));

  	if (header_end == std::string::npos) break;
  	pos = header_end + 2;
  }

  // parse the body
  req->SetBody(body_str);
  return req;
}

const std::string ok =
  "HTTP/1.0 200 OK\r\n";
const std::string bad_request =
  "HTTP/1.0 400 Bad Request\r\n";
const std::string not_found =
  "HTTP/1.0 404 Not Found\r\n";
const std::string internal_server_error =
  "HTTP/1.0 500 Internal Server Error\r\n";

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
  default:
    return internal_server_error;
  }
}

std::string
Response::ToString()
{
  std::string str = "";
  str += status_to_string(status);
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

std::map<std::string, RequestHandler* (*)(void)>* request_handler_builders = nullptr;

RequestHandler* 
RequestHandler::CreateByName(const char* type) {
  const auto type_and_builder = request_handler_builders->find(type);
  if (type_and_builder == request_handler_builders->end()) {
    return nullptr;
  }
  return (*type_and_builder->second)();
}

} // namespace server
} // namespace http

