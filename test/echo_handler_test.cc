#include <string>
#include "gtest/gtest.h"
#include "../src/echo_handler.h"

namespace http {
namespace server {

class EchoHandleRequest: public::testing::Test
{
protected:
	EchoHandleRequest() {
		handler.Init(uri_prefix, config);
	}

	RequestHandler::Status
	HandleRequest(const Request& request, Response* response) 
	{
		return handler.HandleRequest(request, response);
	}

private:
	EchoHandler handler;
	std::string uri_prefix = "/echo";
	NginxConfig config;
};

TEST_F(EchoHandleRequest, SimpleHandleRequest) {
	Request request;
	Response* response = new Response();
	request.SetRawRequest("test");
	EXPECT_EQ(HandleRequest(request, response), RequestHandler::ok);
	std::string response_return = response->ToString();
	std::string response_expect = "HTTP/1.1 200 OK\r\nContent-Length: 4\r\nContent-Type: text/plain\r\n\r\n" + 
								   request.raw_request();
	EXPECT_EQ(response_return, response_expect);

	delete response;
} 

} // namespace server
} // namespace http



