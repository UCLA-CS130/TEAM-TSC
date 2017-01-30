#include "config_handler.h"
#include "config_parser.h"

//Sets up config based on config file. Returns false
//if there are any errors
bool config_handler::setup_config(config_opts& server_config, const char* filename)
{
  if (config_parser.Parse(filename, &config)) {
    server_config.port = config.GetConfigPort();
    if(server_config.port==""){
      std::cerr<<"Error: Invalid port number in config file.\n";
      return false;
    }
  }
  else {
    std::cerr << "Error: Could not parse config file.\n";
    return false;
  }
  return true;

}
