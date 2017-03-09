#ifndef CONFIG_OPTS_H
#define CONFIG_OPTS_H

#include <string>
#include <vector>
#include "config_parser.h"

struct config_opts
{
 public:
  std::string port = "8080"; //Defaults to 8080
  std::string address = "0.0.0.0";
  int threadCount = 10;
  std::vector<std::string> static_file_uri_prefixes;
  std::vector<NginxConfig> static_file_config;
  std::vector<std::string> echo_uri_prefixes;
  std::vector<std::string> status_uri_prefixes;
  //Add other options below
  std::vector<std::string> proxy_uri_prefixes;
  std::vector<NginxConfig> proxy_config;
};

#endif //CONFIG_OPTS_H
