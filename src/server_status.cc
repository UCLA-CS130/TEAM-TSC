#include "server_status.h"

#include <string>
#include <vector>
#include <map>

namespace http{
namespace server{

long long
ServerStatus::getRecordNum(){
	return total_visit;
}

std::string
ServerStatus::responseToString(const Response::ResponseCode& response_code){
	switch(response_code){
	case  Response::ResponseCode::ok:
	  return "200";
	case  Response::ResponseCode::bad_request:
	  return "400";
	case Response::ResponseCode::not_found:
	  return "404";
	default:
	  return "500";
	}
}

void
ServerStatus::addHandlerToUri(const std::string& handlerName, const std::string& uri){
	handler_to_uri[handlerName].push_back(uri);
}

void 
ServerStatus::insertRecord(const std::string& uri, const Response::ResponseCode& response_code)
{
	if (response_code == Response::ok)
		uri_visit_count[uri]++;
	else uri_visit_count["uri with unsuccessful responses"]++;
	status_code_count[responseToString(response_code)]++;
	total_visit++;
}

std::string 
ServerStatus::getStatusString(){
	std::string result = "";
	result += "Information on the status of web server:\n";
	result += "Total request received : " + std::to_string( getRecordNum()) + "\n";
	result += "\n";

	result += "Count of url requested:\n";
	for(auto it = uri_visit_count.begin();it != uri_visit_count.end();it++){
		result += it->first+": "+std::to_string(it->second)+"\n";
	}
	result+="\n";

	result += "Count of status code returned:\n";
	for(auto it = status_code_count.begin(); it!=status_code_count.end();it++){
		result += it->first + ": " + std::to_string(it->second) + "\n";
	}
	result += "\n";


	result += "Existing handlers and their matching uri prefix: \n";
	for(auto it = handler_to_uri.begin();it != handler_to_uri.end();it++){
		std::string handlerName = it -> first;
		result += handlerName + ":";
		for (std::string uri : it -> second){
			result += " " + uri;
		}
		result += "\n";
	}
	return result;
}

} // namespace server
} // namespace http
