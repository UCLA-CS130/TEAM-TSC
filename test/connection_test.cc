#include <string>
#include "gtest/gtest.h"
#include "../src/connection.h"

namespace http {
namespace server {

class TestHandler1: public RequestHandler
{
    RequestHandler::Status
    Init(const std::string& uri_prefix, const NginxConfig& config) {
        return RequestHandler::ok;
    }

    RequestHandler::Status
    HandleRequest(const Request& request, Response* response) {
        return RequestHandler::ok;
    }
};

class TestHandler2: public RequestHandler
{
    RequestHandler::Status
    Init(const std::string& uri_prefix, const NginxConfig& config) {
        return RequestHandler::ok;
    }

    RequestHandler::Status
    HandleRequest(const Request& request, Response* response) {
        return RequestHandler::handle_fail;
    }
};

class ConnectionProcessRequest: public::testing::Test
{
protected:
    bool ProcessRequest(std::string uri_prefix) {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::socket socket(io_service);
        std::map<std::string, std::unique_ptr<RequestHandler>> handlers;
        handlers["/handler1"] = std::unique_ptr<RequestHandler>(new TestHandler1());
        handlers["/handler"] = std::unique_ptr<RequestHandler>(new TestHandler2());
        handlers["/handler/1"] = std::unique_ptr<RequestHandler>(new TestHandler1());
        handlers["/handler/foo/bar"] = std::unique_ptr<RequestHandler>(new TestHandler2());

        std::shared_ptr<Connection> conn = std::make_shared<Connection>(std::move(socket), handlers);
        return conn->ProcessRequest(uri_prefix);
    }
};

TEST_F(ConnectionProcessRequest, SimpleRequestHandle) {
    EXPECT_TRUE(ProcessRequest("/handler1"));
    EXPECT_FALSE(ProcessRequest("/handler"));
}

TEST_F(ConnectionProcessRequest, LongestMatch) {
    EXPECT_TRUE(ProcessRequest("/handler/1"));
    EXPECT_FALSE(ProcessRequest("/handler/foo/bar"));
}

TEST_F(ConnectionProcessRequest, NoneMatch) {
    EXPECT_FALSE(ProcessRequest("/handler/foo"));
    EXPECT_FALSE(ProcessRequest("/handler3"));
}

class ConnectionHandleIO: public::testing::Test
{
protected:
    ConnectionHandleIO() {
        // The handle_read/handle_write must have a 'ErrorHandler'
        handlers["ErrorHandler"] = std::unique_ptr<RequestHandler>(new TestHandler1());
    }

    bool HandleRead(const boost::system::error_code& ec)
    {
        boost::asio::ip::tcp::socket socket(io_service);
        std::shared_ptr<Connection> conn = std::make_shared<Connection>(std::move(socket), handlers);
        return conn->handle_read(ec, 0);
    }

    bool HandleWrite(const boost::system::error_code& ec)
    {
        boost::asio::ip::tcp::socket socket(io_service);
        std::shared_ptr<Connection> conn = std::make_shared<Connection>(std::move(socket), handlers);
        return conn->handle_write(ec, 0);
    } 

private:
    boost::asio::io_service io_service;
    std::map<std::string, std::unique_ptr<RequestHandler>> handlers;
};

TEST_F(ConnectionHandleIO, HandleRead) {
    boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);
    EXPECT_TRUE(HandleRead(ec_success)); 

    boost::system::error_code ec_broken_pipeline = boost::system::errc::make_error_code(boost::system::errc::broken_pipe);
    EXPECT_FALSE(HandleRead(ec_broken_pipeline));    

    boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
    EXPECT_FALSE(HandleRead(ec_connection_aborted));   
}

TEST_F(ConnectionHandleIO, HandleWrite) {
    boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);
    EXPECT_TRUE(HandleWrite(ec_success)); 

    boost::system::error_code ec_broken_pipeline = boost::system::errc::make_error_code(boost::system::errc::broken_pipe);
    EXPECT_FALSE(HandleRead(ec_broken_pipeline));    

    boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
    EXPECT_FALSE(HandleRead(ec_connection_aborted));   
}

} // namespace server
} // namespace http







