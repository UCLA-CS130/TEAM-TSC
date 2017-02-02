#ifndef CONFIG_PARSER_MOCK_H
#define CONFIG_PARSER_MOCK_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/config_parser.h"

class MockConfigParser: public ConfigParserInterface {
public:
	MOCK_METHOD0(run, void());
  	MOCK_METHOD0(do_accept, void());
};

#endif // CONFIG_PARSER_MOCK_H