#include <fstream>
#include <sstream>
#include <string>
#include <boost/log/trivial.hpp>
#include "static_file_handler.h"

namespace http {
namespace server {

RequestHandler::Status
StaticFileHandler::Init(const std::string& uri_prefix_, const NginxConfig& config)
{
  uri_prefix = uri_prefix_;
  int root_num = 0;
  for (auto statement: config.statements_) {
    std::string start_token = statement->tokens_[0];
    if (start_token == "root") {
      if (root_num > 0 || 
          statement->tokens_.size() != 2 ||
          statement->tokens_[1].back() == '/') 
      {
        return RequestHandler::invalid_root_format;
      }
      base_dir = statement->tokens_[1];
      // otherwise the filepath is //...
      if (base_dir == "/") base_dir += ".";
      root_num++;
    }
    // other statements
  }
  return RequestHandler::ok;
}

std::string
StaticFileHandler::extension2type(std::string extension) {
  if (extension == "gif") return "image/gif";
  else if (extension == "htm") return "text/html";
  else if (extension == "html") return "text/html";
  else if (extension == "jpg") return "image/jpeg";
  else if (extension == "png") return "image/png";
  else return "text/plain";
}

RequestHandler::Status 
StaticFileHandler::HandleRequest(const Request& request, Response* response) {
  std::string request_uri = request.uri();

  if (request_uri[request_uri.size() - 1] == '/') {
    request_uri += "index.html";
  }

  std::string file_path = base_dir + request_uri.substr(uri_prefix.size());

  // Determine the file extension.
  std::size_t last_slash_pos = request_uri.find_last_of("/");
  std::size_t last_dot_pos = request_uri.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
    extension = request_uri.substr(last_dot_pos + 1);
  }

  // Open the file to send back.
  std::ifstream is(file_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
    BOOST_LOG_TRIVIAL(error) << "Not found file: " << file_path.c_str() << "\n";
    response->SetStatus(Response::not_found);
    return RequestHandler::handle_fail;
  }

  // Fill out the response to be sent to the client.
  char buf[512];
  std::string body = "";
  while (is.read(buf, sizeof(buf)).gcount() > 0)
    body.append(buf, is.gcount());

  response->SetStatus(Response::ok);
  response->SetBody(body);
  response->AddHeader("Content-Length", std::to_string(body.size()));
  response->AddHeader("Content-Type", extension2type(extension));
  BOOST_LOG_TRIVIAL(trace) << "finish static handle";
  return RequestHandler::ok;
}

} // namespace server
} // namespace http