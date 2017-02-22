#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>
#include "request.h"

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

} // namespace server
} // namespace http
