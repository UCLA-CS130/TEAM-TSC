#include <string>
#include <fstream>
#include <boost/log/trivial.hpp>
#include "../src/proxy_handler.h"
#include "gtest/gtest.h"


//unit test for check_serve_path function 
namespace http {
namespace server {

TEST(ProxyHandlerInit, SimpleRoot) {
	ProxyHandler handler;
	std::string uri_prefix = "/proxy1";
	NginxConfig config;
	config.statements_.emplace_back(new NginxConfigStatement);
	config.statements_.back().get()->tokens_.push_back("host");
	config.statements_.back().get()->tokens_.push_back("wwww.google.com");

	config.statements_.emplace_back(new NginxConfigStatement);
	config.statements_.back().get()->tokens_.push_back("port");
	config.statements_.back().get()->tokens_.push_back("80");

	EXPECT_EQ(handler.Init(uri_prefix, config), 
		      RequestHandler::ok);
}

TEST(ProxyHandlerInit, NoHostName) {
	ProxyHandler handler;
	std::string uri_prefix = "/proxy1";
	NginxConfig config;

	config.statements_.emplace_back(new NginxConfigStatement);
	config.statements_.back().get()->tokens_.push_back("port");
	config.statements_.back().get()->tokens_.push_back("80");

	EXPECT_EQ(handler.Init(uri_prefix, config), 
		      RequestHandler::invalid_config);
}

TEST(ProxyHandlerInit, NoPort) {
	ProxyHandler handler;
	std::string uri_prefix = "/proxy1";
	NginxConfig config;

	config.statements_.emplace_back(new NginxConfigStatement);
	config.statements_.back().get()->tokens_.push_back("host");
	config.statements_.back().get()->tokens_.push_back("www.ucla.edu");

	EXPECT_EQ(handler.Init(uri_prefix, config), 
		      RequestHandler::invalid_config);
}

TEST(ProxyHandlerInit, BadPortNum) {
	ProxyHandler handler;
	std::string uri_prefix = "/proxy1";
	NginxConfig config;

	config.statements_.emplace_back(new NginxConfigStatement);
	config.statements_.back().get()->tokens_.push_back("port");
	config.statements_.back().get()->tokens_.push_back("not a valid number");

	EXPECT_EQ(handler.Init(uri_prefix, config), 
		      RequestHandler::invalid_config);
}

TEST(ProxyHandlerInit, EmptyConfig) {
	ProxyHandler handler;
	std::string uri_prefix = "/proxy1";
	NginxConfig config;
	EXPECT_EQ(handler.Init(uri_prefix, config), 
		      RequestHandler::invalid_config);
}

class ProxyHandleRequest: public::testing::Test
{
protected:
	ProxyHandleRequest() {
		config.statements_.emplace_back(new NginxConfigStatement);
		config.statements_.back().get()->tokens_.push_back("host");
		config.statements_.back().get()->tokens_.push_back("www.google.com");
		config.statements_.back().get()->tokens_.push_back("port");
		config.statements_.back().get()->tokens_.push_back("80");
		handler.Init(uri_prefix, config);
	}

	RequestHandler::Status
	HandleRequest(const Request& request, Response* response) 
	{
		return handler.HandleRequest(request, response);
	}

private:
	ProxyHandler handler;
	std::string uri_prefix = "/proxy1";
	NginxConfig config;
};

TEST_F(ProxyHandleRequest, SimpleHandleRequest) {
	Request request;
	Response* response = new Response();
	request.SetMethod("GET");
	request.SetUri("/proxy1/");
	request.SetVersion("HTTP/1.1");

	EXPECT_EQ(HandleRequest(request, response), RequestHandler::ok);
	delete response;
}

TEST_F(ProxyHandleRequest, ResourceNotFound) {
	Request request;
	Response* response = new Response();
	request.SetMethod("GET");
	request.SetUri("/proxy1/thisisprobablynotavalidresource");
	request.SetVersion("HTTP/1.1");

	EXPECT_EQ(HandleRequest(request, response), RequestHandler::handle_fail);
	delete response;
}

} // namespace server
} // namespace http


