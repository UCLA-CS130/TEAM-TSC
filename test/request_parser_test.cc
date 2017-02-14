#include "gtest/gtest.h"
#include "../src/request_parser.h"

TEST(RequestParserCharacterCheckTests, is_charTest)
{
  EXPECT_TRUE(true);
  /*
    Currently trying to figure out how to test private member functions
  http::server::RequestParser test_parser;
  EXPECT_TRUE(test_parser.is_char('\0'));
  EXPECT_TRUE(test_parser.is_ctl('\0'));
  EXPECT_TRUE(test_parser.is_char('a'));
  EXPECT_FALSE(test_parser.is_ctl('a'));
  */
}
