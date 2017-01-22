#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"

int main(int argc, char* argv[])
{
  try {
    if (argc != 3)
    {
      std::cerr << "Usage: http_server <address> <port>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 .\n";
      return 1;
    }

    http::server::server s(argv[1], argv[2]);

    s.run();
  }
  catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}