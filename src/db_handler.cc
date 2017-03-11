#include "db_handler.h"

/* Standard C++ headers */
//#include <iostream>
//#include <sstream>
//#include <memory>
#include <string>
//#include <stdexcept>
#include <boost/log/trivial.hpp>
#include <boost/regex.hpp>
    
#define DBHOST "tcp://127.0.0.1:3306"
#define USER "root"
#define PASSWORD ""
#define DATABASE "TSC"

namespace http {
namespace server {

RequestHandler::Status
DbHandler::Init(const std::string& uri_prefix_, const NginxConfig& config)
{
  uri_prefix = uri_prefix_;
  driver = get_driver_instance();
  return RequestHandler::ok;
}

bool
DbHandler::QueryTable(const std::string& query, Response *response) 
{
  try {
  	sql::Connection *conn = driver->connect(DBHOST, USER, PASSWORD);
  	conn->setSchema("TSC");

  	sql::Statement *stmt = conn->createStatement();
  	sql::ResultSet *res = stmt->executeQuery(query);

  	std::string rep = "MySQL Query Replies: \n";
  	while (res->next()) {
      /* Access column data by alias or column name */
      rep += std::to_string(res->getInt("x")) + " ";
      rep += std::to_string(res->getInt("y")) + "\n";
    }
    response->SetStatus(Response::ok);
    response->AddHeader("Content-Length", std::to_string(rep.size()));
    response->AddHeader("Content-Type", "text/plain");
    response->SetBody(rep);
    delete res;
    delete stmt;
    delete conn;
    return true;
  } 
  catch (sql::SQLException &e) {
  	//BOOST_LOG_TRIVIAL(trace) << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode();
  	std::string rep = std::string("ERR: ") + std::string(e.what());
  	response->SetStatus(Response::ok);
    response->AddHeader("Content-Length", std::to_string(rep.size()));
    response->AddHeader("Content-Type", "text/plain");
    response->SetBody(rep);
  	return false;
  }
}

bool
DbHandler::UpdateTable(const std::string& update, Response *response) 
{
  try {
  	sql::Connection *conn = driver->connect(DBHOST, USER, PASSWORD);
  	conn->setSchema("TSC");

  	sql::Statement *stmt = conn->createStatement();
  	stmt->executeUpdate(update);

  	std::string rep = "Update the table!";
    response->SetStatus(Response::ok);
    response->AddHeader("Content-Length", std::to_string(rep.size()));
    response->AddHeader("Content-Type", "text/plain");
    response->SetBody(rep);

    delete stmt;
    delete conn;
    return true;
  } 
  catch (sql::SQLException &e) {
  	BOOST_LOG_TRIVIAL(trace) << "# ERR: " << e.what() << " (MySQL error code: " << e.getErrorCode();
  	std::string rep = std::string("ERR: ") + std::string(e.what());
  	response->SetStatus(Response::ok);
    response->AddHeader("Content-Length", std::to_string(rep.size()));
    response->AddHeader("Content-Type", "text/plain");
    response->SetBody(rep);
  	return false;
  }
}

RequestHandler::Status
DbHandler::HandleRequest(const Request& request, Response* response) 
{
  std::string req_body = request.body();
  DecodedBody decoded_body;
  if (!ParseBody(req_body, decoded_body)) {
  	BOOST_LOG_TRIVIAL(info) << "DB_HANDLER: Invalid request body format: ";
  	response->SetStatus(Response::bad_request);
  	return RequestHandler::handle_fail;
  }

  // here we only implement one <name, value> body
  if (decoded_body[0].first == "query") {
  	QueryTable(decoded_body[0].second, response);
  }
  else if (decoded_body[0].first == "update") {
  	UpdateTable(decoded_body[0].second, response);
  }
  else {
  	BOOST_LOG_TRIVIAL(info) << "Invalid request body format";
  	response->SetStatus(Response::bad_request);
  	return RequestHandler::handle_fail;
  }
  return RequestHandler::ok;
}

bool 
DbHandler::ParseBody(const std::string& body, DecodedBody& decoded_body)
{
  std::string sub_body = "";
  for (std::size_t i = 0; i <= body.size(); ++i) {
  	if (body[i] == '&' || i == body.size()) {
 	  boost::cmatch sub_body_mat;
	  boost::regex sub_body_expression ("(\\S+)=(\\S*)");
	  if (!boost::regex_match(sub_body.c_str(), sub_body_mat, sub_body_expression)) {
	  	return false;
	  }
	  std::string name = std::string(sub_body_mat[1]);
	  std::string value;
	  if (!Request::uri_decode(std::string(sub_body_mat[2]), value)) {
	  	return false;
	  }
	  decoded_body.push_back(std::pair<std::string, std::string>(name, value));
  	  sub_body = "";
      continue;
  	}
  	else sub_body.push_back(body[i]);
  }
  return true;
}

} // namespace server
} // namespace http









