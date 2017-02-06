#include "gtest/gtest.h"
#include "../src/config_opts.h"
#include "../src/server.h"
#include "request_parser_mock.h"

using namespace http::server;
class ServerTest: public ::testing::Test {
protected:
    bool handle_accept(boost::system::error_code ec) {
      config_opts server_opt;
      RequestParserMock* request_parser_mock = new RequestParserMock();
      Server s(server_opt);
      return s.handle_accept(request_parser_mock, ec);
    }
};

TEST_F(ServerTest, HandleAccept) {
	boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);
  EXPECT_TRUE(handle_accept(ec_success));

  boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
  EXPECT_FALSE(handle_accept(ec_connection_aborted));
}




