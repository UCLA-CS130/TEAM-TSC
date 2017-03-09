#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include <map>
#include "config_opts.h"
#include "config_parser.h"

class ConfigHandler
{
public:
  ConfigHandler(NginxConfigParserInterface& config_parser_): config_parser(config_parser_) {
  };

  bool 
  handle_config(const char* filename);

  bool 
  handle_statements(const std::vector<std::shared_ptr<NginxConfigStatement>>& statements_);

  config_opts& get_config_opt() {
  	return config_opt;
  }

  NginxConfigParserInterface& config_parser;
  NginxConfig config;

private:
  config_opts config_opt;

  std::map<std::string, bool> check_duplicate_path;

  enum StartTokenType {
    TOKEN_PORT = 0,
    TOKEN_PATH = 1,
    TOKEN_SERVER = 2,
    TOKEN_ROOT = 3,
    TOKEN_DEFAULT = 4,
    TOKEN_THREAD= 5,
    TOKEN_INVALID = 10
  };

  StartTokenType to_token_type(std::string token);
  const int MAX_PORT = 65535;

};

#endif // CONFIG_HANDLER_H
