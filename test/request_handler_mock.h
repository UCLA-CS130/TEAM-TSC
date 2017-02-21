#ifndef REQUEST_HANDLER_MOCK_H
#define REQUEST_HANDLER_MOCK_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/request_handler.h"

using namespace http::server;
class RequestHandlerMock: public RequestHandler {
public:
  	MOCK_METHOD2(Init, RequestHandler::Status(const std::string& uri_prefix, const NginxConfig& config));
  	MOCK_METHOD2(HandleRequest, RequestHandler::Status(const Request& request, Response* response));
};

#endif // REQUEST_HANDLER_MOCK_H
