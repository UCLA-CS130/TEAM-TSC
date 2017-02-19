#include "gtest/gtest.h"
#include "../src/request_handler.h"

using namespace http::server;

TEST(Response, ToString)
{
  Response response;
  response.SetStatus(Response::ok);
  response.AddHeader("Content-Length", "6");
  response.AddHeader("Content-Type", "text/plain");
  response.SetBody("hello");

  std::string to_string = response.ToString();
  std::string expected_string = "HTTP/1.0 200 OK\r\nContent-Length: 6\r\nContent-Type: text/plain\r\n\r\nhello";
  EXPECT_EQ(to_string, expected_string);
}

TEST(RequestParse, SimpleRequest) {
	std::string simpleRequest = "GET / HTTP/1.1\r\nHOST: localhost\r\n\r\n";
	std::unique_ptr<Request> request = Request::Parse(simpleRequest);

	EXPECT_TRUE(request);
	if (request) {
	  EXPECT_EQ(request->method(), "GET");
	  EXPECT_EQ(request->uri(), "/");
      EXPECT_EQ(request->version(), "HTTP/1.1");
	  std::vector<std::pair<std::string, std::string>> headers;
      headers.emplace_back(std::make_pair<std::string, std::string>("HOST", "localhost"));
	  EXPECT_EQ(request->headers(), headers);
	}
}

TEST(RequestParse, InvalidRequestLine) {
	std::string invalid_request = "GET  HTTP/1.1\r\nHOST: localhost\r\n\r\n";
	EXPECT_FALSE(Request::Parse(invalid_request));

	invalid_request = "GET / HTTP/1\r\nHOST: localhost\r\n\r\n";
	EXPECT_FALSE(Request::Parse(invalid_request));	

	invalid_request = "GET  HTTP/1.1\r\nHOST: localhost\r\n\r\n";
	EXPECT_FALSE(Request::Parse(invalid_request));

	invalid_request = " GET / HTTP/1.1\r\nHOST: localhost\r\n\r\n";
	EXPECT_FALSE(Request::Parse(invalid_request));

	invalid_request = "GET / /1.1\r\nHOST: localhost\r\n\r\n";
	EXPECT_FALSE(Request::Parse(invalid_request));
}


