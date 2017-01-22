#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"
#include "config_parser.h"

void config_handler(NginxConfig* config, std::string& port) {
  for(auto statement: config->statements_) {
    if(statement->child_block_ != NULL) config_handler(statement->child_block_);
    if(statement->tokens_[0] == "listen") port = statement->tokens_[1]
  }
}

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
      config_handler(&config, port);
    }
    else {
      std::cerr << "Error: Could not parse config file.\n";
      return 1;
    }

    http::server::server s("localhost", port);
    s.run();
  }
  catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}