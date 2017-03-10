#include <boost/regex.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
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
  boost::regex request_expression ("([^\r\n]*)\r\n(.+)\r\n\r\n(.*)");
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
  // seperate the headers by '\r\n'
  boost::regex delimeter("\r\n");  
  boost::sregex_token_iterator itrBegin(headers_str.begin(), headers_str.end(), delimeter, -1);
  boost::sregex_token_iterator itrEnd;
  boost::cmatch header_mat;
  boost::regex header_expression ("(\\S+)\\s*:\\s*(\\S[^\r\n]*)");
  for(boost::sregex_token_iterator itr = itrBegin; itr != itrEnd; ++itr) {
    std::string header_str = *itr;
    if (!boost::regex_match(header_str.c_str(), header_mat, header_expression)) {
      BOOST_LOG_TRIVIAL(info) << "Invalid header format\n";
      return nullptr;
    }
    req->AddHeader(std::string(header_mat[1]), std::string(header_mat[2]));
  }

  // parse the body
  req->SetBody(body_str);
  return req;
}

bool 
Request::uri_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<char>(value);
          i += 2;
        }
        else return false;
      }
      else return false;
    }
    else if (in[i] == '+') out += ' ';
    else out += in[i];
  }
  return true;
}

std::string
Request::ToString() const
{
  std::string str = method_ + " " + uri_ + " " + version_ + "\r\n";
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
