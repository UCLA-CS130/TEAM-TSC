#ifndef REQUEST_PARSER_MOCK_H
#define REQUEST_PARSER_MOCK_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/request_parser_interface.h"

using namespace http::server;
class RequestParserMock: public RequestParserInterface {
public:
  	MOCK_METHOD3(parse, std::tuple<result_type, char*>(request& req, char* begin, char* end));
};

#endif // CONFIG_PARSER_MOCK_H
