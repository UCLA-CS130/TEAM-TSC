#include <fstream>
#include <sstream>
#include <string>
#include "static_request_handler.h"

namespace http {
namespace server {


StaticRequestHandler::StaticRequestHandler(const handler_opts& handler_opts_):
  serve_paths(handler_opts_.paths),
  uri_root2base_dir(handler_opts_.uri_root2base_dir)
{
}

std::string
StaticRequestHandler::extension2type(std::string extension) {
  if (extension == "gif") return "image/gif";
  else if (extension == "htm") return "text/html";
  else if (extension == "html") return "text/html";
  else if (extension == "jpg") return "image/jpeg";
  else if (extension == "png") return "image/png";
  else return "text/plain";
}

bool 
StaticRequestHandler::handle_request(const std::string req_str, const request& req, reply& rep) {
  std::string request_uri = req.uri;

  if (request_uri[request_uri.size() - 1] == '/') {
    request_uri += "index.html";
  }


  std::string full_path;
  // Request path must be absolute and not contain "..".
  for (auto it = uri_root2base_dir.begin(); it != uri_root2base_dir.end(); ++it) {
    std::string uri_root = it->first;
    if (request_uri.find(uri_root) == 0) {
      // invalid if open a directory 
      if (request_uri.size() == uri_root.size()) {
        rep = reply::stock_reply(reply::not_found);
        return false;
      }
      full_path = it->second + request_uri.substr(uri_root.size());
      break;
    }
  }

  // Determine the file extension.
  std::size_t last_slash_pos = request_uri.find_last_of("/");
  std::size_t last_dot_pos = request_uri.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
    extension = request_uri.substr(last_dot_pos + 1);
  }

  // Open the file to send back.

  std::cout<<"full_path: "<<full_path<<std::endl;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
    rep = reply::stock_reply(reply::not_found);
    return false;
  }

  // Fill out the reply to be sent to the client.
  rep.status = reply::ok;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0)
    rep.content.append(buf, is.gcount());

  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = extension2type(extension);
  return true;
}

bool 
StaticRequestHandler::check_serve_path(std::string uri) {
  for (auto i: serve_paths) {
    std::size_t found = uri.find(i);
    if (found == 0) {
      return true;
    }
  }
  return false;
}

} // namespace server
} // namespace http