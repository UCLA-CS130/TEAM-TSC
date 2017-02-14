#ifndef REQUEST_PARSER_MOCK_H
#define REQUEST_PARSER_MOCK_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/request_parser.h"

using namespace http::server;
class RequestParserMock: public RequestParser {
public:
	//RequestParserMock(): RequestParser() {}
  	MOCK_METHOD3(parse, std::tuple<result_type, char*>(request& req, char* begin, char* end));
};

#endif // REQUEST_PARSER_MOCK_H
