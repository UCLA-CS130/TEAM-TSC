#include <string>
#include <fstream>
#include <boost/log/trivial.hpp>
#include "../src/static_file_handler.h"
#include "gtest/gtest.h"


//unit test for check_serve_path function 
namespace http {
namespace server {

TEST(StaticFileHandlerInit, SimpleRoot) {
	StaticFileHandler handler;
	std::string uri_prefix = "/static";
	NginxConfig config;
	config.statements_.emplace_back(new NginxConfigStatement);
	config.statements_.back().get()->tokens_.push_back("root");
	config.statements_.back().get()->tokens_.push_back("/test");
	EXPECT_EQ(handler.Init(uri_prefix, config), 
		      RequestHandler::ok);
}

TEST(StaticFileHandlerInit, TooManyRoot) {
	StaticFileHandler handler;
	std::string uri_prefix = "/static";
	NginxConfig config;

	config.statements_.emplace_back(new NginxConfigStatement);
	config.statements_.back().get()->tokens_.push_back("root");
	config.statements_.back().get()->tokens_.push_back("/test1");

	config.statements_.emplace_back(new NginxConfigStatement);
	config.statements_.back().get()->tokens_.push_back("root");
	config.statements_.back().get()->tokens_.push_back("/test2");
	EXPECT_EQ(handler.Init(uri_prefix, config), 
		      RequestHandler::invalid_root_format);
}

TEST(StaticFileHandlerInit, InvalidRoot) {
	StaticFileHandler handler;
	std::string uri_prefix = "/static";
	NginxConfig config;
	config.statements_.emplace_back(new NginxConfigStatement);
	config.statements_.back().get()->tokens_.push_back("root");
	config.statements_.back().get()->tokens_.push_back("test/");
	EXPECT_EQ(handler.Init(uri_prefix, config), 
		      RequestHandler::invalid_root_format);
}

class StaticHandleRequest: public::testing::Test
{
protected:
	StaticHandleRequest() {
		config.statements_.emplace_back(new NginxConfigStatement);
		config.statements_.back().get()->tokens_.push_back("root");
		config.statements_.back().get()->tokens_.push_back(".");
		handler.Init(uri_prefix, config);
	}

	RequestHandler::Status
	HandleRequest(const Request& request, Response* response) 
	{
		return handler.HandleRequest(request, response);
	}

private:
	StaticFileHandler handler;
	std::string uri_prefix = "/static";
	NginxConfig config;
};

TEST_F(StaticHandleRequest, SimpleHandleRequest) {
	Request request;
	Response* response = new Response();
	request.SetMethod("GET");
	request.SetUri("/static/test.txt");
	request.SetVersion("HTTP/1.1");

	// build a template file:
	const char* tmp_file = "test.txt";
	std::ofstream fout(tmp_file);
	if (fout) {
		fout << "test!"; 
		fout.close();
	}
	EXPECT_EQ(HandleRequest(request, response), RequestHandler::ok);
	delete response;
	// remove the tmp_file
	if (remove(tmp_file) != 0) {
		BOOST_LOG_TRIVIAL(error) << "Could not remove the tmp_file\n";
	}
}

TEST_F(StaticHandleRequest, FileNotFound) {
	Request request;
	Response* response = new Response();
	request.SetMethod("GET");
	request.SetUri("/static/test.txt");
	request.SetVersion("HTTP/1.1");

	EXPECT_EQ(HandleRequest(request, response), RequestHandler::handle_fail);
	delete response;
}

} // namespace server
} // namespace http


