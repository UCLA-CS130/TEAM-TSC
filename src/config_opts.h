#ifndef CONFIG_OPTS_H
#define CONFIG_OPTS_H

#include <string>
#include <vector>
#include <map>

struct handler_opts
{
 public:
  std::vector<std::string> paths;
  std::map<std::string, std::string> uri_root2base_dir;
};

struct config_opts
{
 public:
  std::string port = "8080"; //Defaults to 8080
  std::string address = "0.0.0.0";
  handler_opts static_handler;
  handler_opts echo_handler;
  //Add other options below

};

#endif //CONFIG_OPTS_H
