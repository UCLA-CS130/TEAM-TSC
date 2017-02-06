#include "../src/static_request_handler.h"
#include <string>
#include "../src/request.h"
#include "../src/reply.h"
#include "gtest/gtest.h"


//unit test for check_serve_path function 
namespace http {
namespace server {
class StaticRequestHandlerTest : public::testing::Test
{
protected:
	std::vector<std::string> static_path;
	std::map<std::string,std::string> url_root2base_dir_;
	StaticRequestHandler* staticHandler;
	request req;
	reply rep;

	StaticRequestHandlerTest()
	{
		static_path.push_back("/static1");
		static_path.push_back("/static2");
		url_root2base_dir_.insert(std::make_pair("/static1","content1"));
		url_root2base_dir_.insert(std::make_pair("/static2","content2"));
		staticHandler = new StaticRequestHandler(static_path,url_root2base_dir_);
	}

	~StaticRequestHandlerTest()
	{
		if(staticHandler != NULL) delete staticHandler;
	}

	bool validRequest(std::string uri)
	{
		req.uri = uri;
		std::string requestString = "";
		return staticHandler->handle_request(requestString,req,rep);
	}
};

TEST_F(StaticRequestHandlerTest,ValidStaticPath)
{
	EXPECT_TRUE(staticHandler->check_serve_path("/static1"));
	EXPECT_TRUE(staticHandler->check_serve_path("/static2/echo1.txt"));
}

TEST_F(StaticRequestHandlerTest,InvalidStaticPath)
{
	EXPECT_FALSE(staticHandler->check_serve_path("/echo1"));
	EXPECT_FALSE(staticHandler->check_serve_path("static1"));
}


//unit test for extension2type function
TEST_F(StaticRequestHandlerTest, extension2TypeTest)
{
	EXPECT_EQ(staticHandler->extension2type("csv"),"text/plain");
	EXPECT_EQ(staticHandler->extension2type("pdf"),"text/plain");
	EXPECT_EQ(staticHandler->extension2type("htm"),"text/html");
}


TEST_F(StaticRequestHandlerTest,HandleRequest)
{
	EXPECT_TRUE(validRequest("/static1/test.html"));
	EXPECT_TRUE(validRequest("/static2/"));
	EXPECT_FALSE(validRequest("/static1"));
	EXPECT_FALSE(validRequest("/static1/homepage.html"));
}
}
}


