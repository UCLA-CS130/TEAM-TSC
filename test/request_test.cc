#include <memory>
#include "gtest/gtest.h"
#include "../src/request.h"

namespace http {
namespace server {

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

} // namespace server
} // namespace http


