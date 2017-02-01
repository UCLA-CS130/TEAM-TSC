#ifndef SERVER_TEST_H
#define SERVER_TEST_H

#include "../src/server_interface.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "../src/config_opts.h"

class MockServer
	: public ServerInterface
{
public:
	MOCK_METHOD0(run, void());
  	MOCK_METHOD0(do_accept, void());
};

#endif // SERVER_TEST_H