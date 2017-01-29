#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.h"
#include "config_parser.h"


int main(int argc, char* argv[])
{
  try {
    if (argc != 2)
    {
      std::cerr << "Usage: ./webserver <path to config file>\n";
      return 1;
    }

    NginxConfigParser config_parser;
    NginxConfig config;
    std::string port;
    
    if (config_parser.Parse(argv[1], &config)) {
      port = config.GetConfigPort();
      if(port==""){
        std::cerr<<"Error: Invalid port number in config file.\n";
        return 1;
      }
    }
    else {
      std::cerr << "Error: Could not parse config file.\n";
      return 1;
    }

    http::server::server s("0.0.0.0", port);
    s.run();
  }
  catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
