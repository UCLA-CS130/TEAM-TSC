#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include "config_opts.h"
#include "config_parser.h"

class ConfigHandler
{
public:
  ConfigHandler(NginxConfigParserInterface& config_parser_): config_parser(config_parser_) {
  };

  bool setup_config(const char* filename);

  bool setup_config(const std::vector<std::shared_ptr<NginxConfigStatement>>& statements_);

  config_opts& get_config_opt() {
  	return config_opt;
  }

  NginxConfigParserInterface& config_parser;
  NginxConfig config;

private:
  config_opts config_opt;

  enum StartTokenType {
    TOKEN_BEFORE_PORT = 0,
    TOKEN_BEFORE_HANDLER = 1,
    TOKEN_SERVER = 2,
    TOKEN_BASE_PATH = 3,
    TOKEN_INVALID = 10
  };

  StartTokenType to_token_type(std::string token);
  const int MAX_PORT = 65535;

};

#endif // CONFIG_HANDLER_H
