#ifndef HTTP_SERVER_STATUS_H
#define HTTP_SERVER_STATUS_H

#include <string>
#include <vector>
#include <map>





namespace http{
namespace server{

enum ResponseCode {
// Define your HTTP response codes here.
ok = 200,
bad_request = 400,
not_found = 404,
internal_server_error = 500
};



class ServerStatus{
public:
	static ServerStatus& getInstance(){
		static ServerStatus theServerStatus;
		return theServerStatus;
	}

	void addHandlerToUri(std::string handlerName, std::string uri);

	void addRecord(std::string uri, ResponseCode response);

	long long getRecordNum();

	std::string responseToString(ResponseCode response);

	std::string getStatusString();

private:

	ServerStatus(){total_visit=0;};
	//static std::vector<Record> records;
	static std::map<std::string,long long> uri_visit_count;
	static std::map<std::string,long long> status_code_count;
	static std::map<std::string,std::vector<std::string>> handler_to_uri;
	static long long total_visit;

};


}
}


#endif