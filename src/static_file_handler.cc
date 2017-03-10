#include <fstream>
#include <sstream>
#include <string>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include "static_file_handler.h"
#include "../cpp-markdown/markdown.h"

namespace http {
namespace server {

RequestHandler::Status
StaticFileHandler::Init(const std::string& uri_prefix, const NginxConfig& config)
{
  uri_prefix_ = uri_prefix;
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
  std::string request_uri;

  if (!Request::uri_decode(request.uri(), request_uri)) {
    BOOST_LOG_TRIVIAL(trace) << "URI decode error";
    response->SetStatus(Response::bad_request);
    return RequestHandler::handle_fail;
  }

  std::string file_path = base_dir + '/' + request_uri.substr(uri_prefix_.size());

  if (file_path[file_path.size() - 1] == '/') {
    file_path += "index.html";
  }

  // Determine the file extension.
  std::size_t last_slash_pos = file_path.find_last_of("/");
  std::size_t last_dot_pos = file_path.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
    extension = file_path.substr(last_dot_pos + 1);
  }

  boost::filesystem::path boost_path(file_path);
  if (!boost::filesystem::exists(file_path) || 
      !boost::filesystem::is_regular_file(file_path)) {
    BOOST_LOG_TRIVIAL(info) << "Not found file: " << file_path.c_str();
    response->SetStatus(Response::not_found);
    return RequestHandler::handle_fail;
  }

  // Open the file to send back.
  std::ifstream is(file_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
    BOOST_LOG_TRIVIAL(info) << "Not found file: " << file_path.c_str();
    response->SetStatus(Response::not_found);
    return RequestHandler::handle_fail;
  }

  // Fill out the response to be sent to the client.
  char buf[512];
  std::string body;
  // Implement Markdown rendering using cpp-markdown library
  if (extension == "md") {
    std::ostringstream os_body;
    markdown::Document doc;
    doc.read(is);
    doc.write(os_body);
    body = os_body.str();
    // convert the extension to 'html'
    extension = "html";
  }
  else {
    while (is.read(buf, sizeof(buf)).gcount() > 0)
    body.append(buf, is.gcount());
  }

  // TODO here: If the url is a directory?
  //if (body.size() == 0) body = "Empty file or Directory!";
  response->SetStatus(Response::ok);
  response->SetBody(body);
  response->AddHeader("Content-Length", std::to_string(body.size()));
  response->AddHeader("Content-Type", extension2type(extension));
  return RequestHandler::ok;
}

} // namespace server
} // namespace http