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
ServerStatus::responseToString(Response::ResponseCode response){
	switch(response){
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
ServerStatus::addHandlerToUri(std::string handlerName, std::string uri){
	handler_to_uri[handlerName].push_back(uri);
}

/*
void 
ServerStatus::addRecord(std::string uri, ResponseCode response){
	if(uri_visit_count.count(uri) <= 0)
		uri_visit_count[uri] = 0;
	uri_visit_count[uri]++;

	if(status_code_count.count(responseToString(response)) <= 0)
		status_code_count[responseToString(response)] = 0;
	status_code_count[responseToString(response)]++;

	total_visit++;
	return;
}
*/

void 
ServerStatus::addUri(std::string uri){
	if(uri_visit_count.count(uri) <= 0)
		uri_visit_count[uri] = 0;
	uri_visit_count[uri]++;	
}


void 
ServerStatus::addStatusCodeAndTotalVisit(Response::ResponseCode response){
	if(status_code_count.count(responseToString(response)) <= 0)
		status_code_count[responseToString(response)] = 0;
	status_code_count[responseToString(response)]++;

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

}
}
