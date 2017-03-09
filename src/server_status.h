#ifndef HTTP_SERVER_STATUS_H
#define HTTP_SERVER_STATUS_H

#include <string>
#include <vector>
#include <map>
#include "response.h"

namespace http{
namespace server{

class ServerStatus{
public:
	static ServerStatus& getInstance(){
		static ServerStatus theServerStatus;
		return theServerStatus;
	}

	void addHandlerToUri(const std::string& handlerName, const std::string& uri);

	void insertRecord(const std::string& uri, const Response::ResponseCode& response_code);

	long long getRecordNum();

	std::string responseToString(const Response::ResponseCode& response_code);

	std::string getStatusString();

private:
	ServerStatus() {
	  total_visit = 0;
	}
	std::map<std::string, long long> uri_visit_count;

	std::map<std::string, long long> status_code_count;

	std::map<std::string, std::vector<std::string>> handler_to_uri;

	long long total_visit;
};

} // namespace server
} // namespace http


#endif // HTTP_SERVER_STATUS_H