#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include "config_opts.h"
#include "config_parser.h"

class config_handler
{
 public:
  bool setup_config(config_opts& server_config, const char* filename);

 private:
  NginxConfigParser config_parser;
  NginxConfig config;
};

#endif // CONFIG_HANDLER_H
