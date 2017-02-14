#ifndef CONFIG_PARSER_MOCK_H
#define CONFIG_PARSER_MOCK_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/config_parser.h"

class NginxConfigParserMock: public NginxConfigParserInterface {
public:
  	MOCK_METHOD2(Parse, bool(const char* file_name, NginxConfig* config));
};

#endif // CONFIG_PARSER_MOCK_H