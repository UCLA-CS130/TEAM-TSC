#ifndef HTTP_SERVER_STATUS_H
#define HTTP_SERVER_STATUS_H

#include <string>
#include <vector>
#include <map>
#include "response.h"





namespace http{
namespace server{

/*
enum ResponseCode {
// Define your HTTP response codes here.
ok = 200,
bad_request = 400,
not_found = 404,
internal_server_error = 500
};
*/


class ServerStatus{
public:
	static ServerStatus& getInstance(){
		static ServerStatus theServerStatus;
		return theServerStatus;
	}

	void addHandlerToUri(std::string handlerName, std::string uri);

	//void addRecord(std::string uri, ResponseCode response);
	void addUri(std::string uri = "Invalid uri");

	void addStatusCodeAndTotalVisit(Response::ResponseCode response);



	long long getRecordNum();

	std::string responseToString(Response::ResponseCode response);

	std::string getStatusString();

private:

	ServerStatus(){total_visit=0;};
	//static std::vector<Record> records;
	std::map<std::string,long long> uri_visit_count;
	std::map<std::string,long long> status_code_count;
	std::map<std::string,std::vector<std::string>> handler_to_uri;
	long long total_visit;

};


}
}


#endif