#include "gtest/gtest.h"
#include "../src/config_handler.h"

TEST(ConfigHandler, InvalidParseFile) {
	ConfigHandler config_handler;
	EXPECT_FALSE(config_handler.setup_config("invalid_parse_file"));
}

TEST(ConfigHandler, WrongPort) {
	ConfigHandler config_handler;
	EXPECT_FALSE(config_handler.setup_config("wrong_port"));
}

TEST(ConfigHandler, SetUpConfigSuccessfully) {
	ConfigHandler config_handler;
	EXPECT_TRUE(config_handler.setup_config("valid_config_file"));
}

