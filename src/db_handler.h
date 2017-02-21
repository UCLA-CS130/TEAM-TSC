#ifndef HTTP_DB_HANDLER_H
#define HTTP_DB_HANDLER_H

/* MySQL Connector/C++ specific headers （all headers are under /usr/include/cppconn/）*/
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/metadata.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/exception.h>
#include <cppconn/warning.h>

#include <string>
#include <vector>
#include "request_handler.h"

namespace http {
namespace server {
  
/// The common handler for all incoming requests.
class DbHandler: public RequestHandler
{
 public:
  DbHandler() {};

  RequestHandler::Status Init(const std::string& uri_prefix_,
                              const NginxConfig& config);

  RequestHandler::Status HandleRequest(const Request& request,
                                       Response* response);

  bool
  QueryTable(const std::string& query, Response *response);

  bool
  UpdateTable(const std::string& update, Response *response);

  using DecodedBody = std::vector<std::pair<std::string, std::string>>;
  bool ParseBody(const std::string& body, DecodedBody& decoded_body);

 private:
  std::string uri_prefix;

  sql::Driver *driver;

  //std::unique_ptr<sql::Connection> conn;

};

REGISTER_REQUEST_HANDLER(DbHandler);

} // server
} // http

#endif // HTTP_DB_HANDLER_H