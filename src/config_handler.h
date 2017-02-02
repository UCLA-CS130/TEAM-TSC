#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include "config_opts.h"
#include "config_parser.h"

class ConfigHandler
{
public:
  ConfigHandler(NginxConfigParserInterface* config_parser_): config_parser(config_parser_) {
    config = new NginxConfig();
  };

  ConfigHandler() {
    config_parser = new NginxConfigParser();
    config = new NginxConfig();
  }

  bool setup_config(const char* filename);

  bool setup_config(const std::vector<std::shared_ptr<NginxConfigStatement>>& statements_);

  config_opts& get_config_opt() {
  	return config_opt;
  }

  NginxConfigParserInterface *config_parser;
  NginxConfig *config;

private:
  config_opts config_opt;

  const std::string TOKEN_BEFORE_PORT = "listen";
  const int MAX_PORT = 65535;

};

#endif // CONFIG_HANDLER_H
