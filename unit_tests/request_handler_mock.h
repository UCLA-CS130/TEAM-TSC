#ifndef REQUEST_HANDLER_MOCK_H
#define REQUEST_HANDLER_MOCK_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/request_handler.h"

using namespace http::server;
class RequestHandlerMock: public RequestHandler {
public:
  	MOCK_METHOD1(check_serve_path, bool(std::string uri));
  	MOCK_METHOD3(handle_request, bool(const std::string req_str, const request& req, reply& rep));
};

#endif // CONFIG_PARSER_MOCK_H
