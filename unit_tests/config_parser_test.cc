#include "gtest/gtest.h"
#include "../src/config_parser.h"


//unit tests for class NginxConfigStatement
TEST(NginxConfigStatementToStringTest, EmptyInput)
{
	NginxConfigStatement statement;
	EXPECT_EQ(statement.ToString(0),";\n");
}

TEST(NginxConfigStatementToStringTest, NormalInput)
{
	NginxConfigStatement statement;
	statement.tokens_.push_back("foo");
	statement.tokens_.push_back("bar");
	EXPECT_EQ(statement.ToString(0),"foo bar;\n");
}


TEST(NginxConfigStatementToStringTest,BlockInput)
{
	NginxConfigStatement statement;
	statement.tokens_.push_back("server");
	NginxConfig* new_config = new NginxConfig;
	statement.child_block_.reset(new_config);
	new_config->statements_.emplace_back(new NginxConfigStatement);
	new_config->statements_.back().get()->tokens_.push_back("port");
	new_config->statements_.back().get()->tokens_.push_back("1080");
	EXPECT_EQ(statement.ToString(0),"server {\n  port 1080;\n}\n");
}



//unit tests for class NginxConfig
TEST(NginxConfigToStringTest,NormalInput)
{
	NginxConfig config;
	for(int i=0;i<2;i++)
	{
		config.statements_.emplace_back(new NginxConfigStatement);
		config.statements_.back().get()->tokens_.push_back("port");
		config.statements_.back().get()->tokens_.push_back("1080");
	}
	EXPECT_EQ(config.ToString(0),"port 1080;\nport 1080;\n");
}

//unit test for class NginxConfigParser
class NginxConfigParserTest : public::testing::Test
{
protected:
	bool ParseString(const std::string configString)
	{
		std::stringstream configStream(configString);
		return parser.Parse(&configStream,&outConfig);
	}

    NginxConfigParser parser;
    NginxConfig outConfig;	
};

TEST_F(NginxConfigParserTest,InvalidSingleInput)
{
	EXPECT_FALSE(ParseString("foo bar"));
}

TEST_F(NginxConfigParserTest,NormalSingleInput)
{
	EXPECT_TRUE(ParseString("worker_processes 4;"));
}

TEST_F(NginxConfigParserTest,normalBlockInput)
{
	EXPECT_TRUE(ParseString("events {worker_connection 768; multi_accept on;}"));
}

/* original config_parser failed here */
TEST_F(NginxConfigParserTest,missMatchBlockInput)
{
	EXPECT_FALSE(ParseString("events {worker_connection 768;")); //error: does not check parantheses matching
	EXPECT_FALSE(ParseString("events worker_connection 768;}")); //error: does not check parantheses matching
	EXPECT_FALSE(ParseString("events {{worker_connection 768;}"));
	EXPECT_FALSE(ParseString("events {worker_connection 768;}}"));
}

/* original config_parser failed here */
TEST_F(NginxConfigParserTest,recurBlockTest)
{
	EXPECT_TRUE(ParseString("events { server {listen 80;}}")); //error: does not allow existence of "}}"
}

TEST_F(NginxConfigParserTest, fileInput) 
{
  bool success = parser.Parse("src/config_file", &outConfig);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, nonexistent_file)
{
  bool success = parser.Parse("src/nonexistent_file", &outConfig);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, folder)
{
  bool success = parser.Parse("src/", &outConfig);
  EXPECT_FALSE(success);
}

