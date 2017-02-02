#include "../src/config_handler.h"
#include "config_parser_mock.h"
#include <string>

class ConfigHandlerTest: public::testing::Test
{
protected:
	bool SetUpConfig(bool config_parser_return, NginxConfigStatement *statement) {
		config_handler  = new ConfigHandler(&mock_config_parser);
		config_handler->config->statements_.emplace_back(statement);
		EXPECT_CALL(mock_config_parser, Parse(config_file_path, config_handler->config))\
		    .Times(::testing::Exactly(1))\
		    .WillOnce(::testing::Return(config_parser_return));
		return config_handler->setup_config(config_file_path);
	}

    MockNginxConfigParser mock_config_parser;
    ConfigHandler *config_handler;
    const char* config_file_path = ".";
};

TEST_F(ConfigHandlerTest, ParseFail) {
	NginxConfigStatement *statement = new NginxConfigStatement;
	statement->tokens_.push_back("listen");
	statement->tokens_.push_back("8080");
	EXPECT_FALSE(SetUpConfig(false, statement));
}

TEST_F(ConfigHandlerTest, SimpleConfig) {
	NginxConfigStatement *statement = new NginxConfigStatement;
	statement->tokens_.push_back("listen");
	statement->tokens_.push_back("8080");
	EXPECT_TRUE(SetUpConfig(true, statement));
}

TEST_F(ConfigHandlerTest, BlockConfig) {
	NginxConfigStatement *statement = new NginxConfigStatement;
	statement->tokens_.push_back("server");

	NginxConfig* child_config = new NginxConfig;
	child_config->statements_.emplace_back(new NginxConfigStatement);
	child_config->statements_.back().get()->tokens_.push_back("listen");
	child_config->statements_.back().get()->tokens_.push_back("1080");
	statement->child_block_.reset(child_config);

	EXPECT_TRUE(SetUpConfig(true, statement));
}

TEST_F(ConfigHandlerTest, TooLargePortNumber) {
	NginxConfigStatement *statement = new NginxConfigStatement;
	statement->tokens_.push_back("listen");
	statement->tokens_.push_back("65537");
	EXPECT_FALSE(SetUpConfig(true, statement));
}

TEST_F(ConfigHandlerTest, InvalidPortNumber) {
	NginxConfigStatement *statement = new NginxConfigStatement;
	statement->tokens_.push_back("listen");
	statement->tokens_.push_back("hgfer");
	EXPECT_FALSE(SetUpConfig(true, statement));
}







