#include <iostream>

#include <string>
#include <boost/asio.hpp>
#include "server.h"
#include "config_opts.h"
#include "config_handler.h"

int main(int argc, char* argv[])
{
  try {
    if (argc != 2)
    {
      std::cerr << "Usage: ./webserver <path to config file>\n";
      return 1;
    }

    std::string port;

    ConfigHandler server_config_handler;
    
    //Attempt to fill in server_config based on the config file
    if(!server_config_handler.setup_config(argv[1])) {
      std::cerr << "config file is wrong\n";
      return 1;
    }
    config_opts server_config = server_config_handler.get_config_opt();

    http::server::Server s(server_config);
    s.run();
  }
  catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
