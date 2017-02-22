#include <iostream>

#include <string>
#include <boost/asio.hpp>
#include "server.h"
#include "server_status.h"

std::map<std::string,long long> http::server::ServerStatus::uri_visit_count;
std::map<std::string,long long> http::server::ServerStatus::status_code_count;
std::map<std::string,std::vector<std::string>> http::server::ServerStatus::handler_to_uri;
long long http::server::ServerStatus::total_visit = 0;

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
