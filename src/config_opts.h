#ifndef CONFIG_OPTS_H
#define CONFIG_OPTS_H

#include <string>
#include <vector>
#include <map>

struct config_opts
{
 public:
  std::string port = "8080"; //Defaults to 8080
  std::string address = "0.0.0.0";
  std::vector<std::string> echo_paths;
  std::vector<std::string> static_paths;
  std::map<std::string, std::string> url_root2base_dir;

  //Add other options below

};

#endif //CONFIG_OPTS_H
