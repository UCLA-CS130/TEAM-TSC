#include "gtest/gtest.h"
#include "../src/response.h"

using namespace http::server;

TEST(Response, ToString)
{
  Response response;
  response.SetStatus(Response::ok);
  response.AddHeader("Content-Length", "6");
  response.AddHeader("Content-Type", "text/plain");
  response.SetBody("hello");

  std::string to_string = response.ToString();
  std::string expected_string = "HTTP/1.1 200 OK\r\nContent-Length: 6\r\nContent-Type: text/plain\r\n\r\nhello";
  EXPECT_EQ(to_string, expected_string);
}