#ifndef HTTP_STATIC_REQUEST_HANDLER_H
#define HTTP_STATIC_REQUEST_HANDLER_H

#include <string>
#include <vector>
#include <map>
#include "request_handler.h"
#include "gtest/gtest_prod.h"
#include "config_opts.h"

namespace http {
namespace server {
  
/// The common handler for all incoming requests.
class StaticRequestHandler: public RequestHandler
{
 public:
  StaticRequestHandler(const handler_opts& hanlder_opts_);

  bool handle_request(const std::string req_str, const request& req, reply& rep);

  bool check_serve_path(std::string uri);

 private:
  std::vector<std::string> serve_paths;

  std::map<std::string, std::string> uri_root2base_dir; 

  //add FRIEND_TEST so we can test this function
  friend class StaticRequestHandlerTest;
  FRIEND_TEST(StaticRequestHandlerTest, extension2TypeTest);
  std::string extension2type(std::string);
};

} // server
} // http

#endif // HTTP_STATIC_REQUEST_HANDLER_H