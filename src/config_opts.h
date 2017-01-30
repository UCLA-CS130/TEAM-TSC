#ifndef CONFIG_OPTS_H
#define CONFIG_OPTS_H

#include <string>

struct config_opts
{
 public:
  std::string port = "8080"; //Defaults to 8080
  std::string address = "0.0.0.0";
  //Add other options below

};

#endif //CONFIG_OPTS_H
