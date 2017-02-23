#include <iostream>

#include <string>
#include <boost/asio.hpp>
#include "server.h"
#include "server_status.h"

int main(int argc, char* argv[])
{
  try {
    if (argc != 2)
    {
      std::cerr << "Usage: ./webserver <path to config file>\n";
      return 1;
    }

    http::server::Server s;
    if (!s.init(argv[1])) {
      std::cerr << "Server Init Error\n";
      return 1;
    }
    s.run();
  }
  catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
