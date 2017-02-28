#include "proxy_handler.h"

namespace http{
	namespace server{
	  RequestHandler::Status ProxyHandler::Init(const std::string& uri_prefix_, const NginxConfig& config){
			uri_prefix = uri_prefix_;
  			//int root_num = 0;
  			for (auto statement: config.statements_) {
    			std::string start_token = statement->tokens_[0];
    			if (start_token == "url") {
      				if(statement->tokens_.size() != 1){
      					url = statement->tokens_[1];
      				}
      				else {
      					//throw error.
      				}
    			}

    			if(start_token == "port"){
    				if(statement->tokens_.size() != 1){
      					portno = statement->tokens_[1];
      				}
      				else {
      					//throw error.
      				}
    			}
    		// other statements
  			}
  			return RequestHandler::ok;
		}

	  RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response){
  			return RequestHandler::ok;
  		}
	}
}
