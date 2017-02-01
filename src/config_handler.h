#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include "config_opts.h"
#include "config_parser.h"

class ConfigHandler
{
public:
  bool setup_config(const char* filename);

  bool setup_config(std::vector<std::shared_ptr<NginxConfigStatement>>& statements_);

  config_opts& get_config_opt() {
  	return config_opt;
  }

private:
  NginxConfigParser config_parser;
  NginxConfig config;

  config_opts config_opt;

  const std::string TOKEN_BEFORE_PORT = "listen";
  const int MAX_PORT = 65535;

};

#endif // CONFIG_HANDLER_H
