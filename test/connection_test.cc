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
    bool ProcessRequest(const Request& request) {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::socket socket(io_service);
        std::map<std::string, std::unique_ptr<RequestHandler>> handlers;
        handlers["/handler1"] = std::unique_ptr<RequestHandler>(new TestHandler1());
        handlers["/handler"] = std::unique_ptr<RequestHandler>(new TestHandler2());
        handlers["/handler/1"] = std::unique_ptr<RequestHandler>(new TestHandler1());
        handlers["/handler/foo/bar"] = std::unique_ptr<RequestHandler>(new TestHandler2());

        std::shared_ptr<Connection> conn = std::make_shared<Connection>(std::move(socket), handlers);
        return conn->ProcessRequest(request);
    }
};

TEST_F(ConnectionProcessRequest, SimpleRequestHandle) {
    Request request;
    request.SetUri("/handler1");
    EXPECT_TRUE(ProcessRequest(request));
    request.SetUri("/handler");
    EXPECT_FALSE(ProcessRequest(request));
}

TEST_F(ConnectionProcessRequest, LongestMatch) {
    Request request;
    request.SetUri("/handler/1");
    EXPECT_TRUE(ProcessRequest(request));
    request.SetUri("/handler/foo/bar");
    EXPECT_FALSE(ProcessRequest(request));
}

TEST_F(ConnectionProcessRequest, NoneMatch) {
    Request request;
    request.SetUri("/handler/foo");
    EXPECT_FALSE(ProcessRequest(request));
    request.SetUri("/handler3");
    EXPECT_FALSE(ProcessRequest(request));
}

class ConnectionHandleIO: public::testing::Test
{
protected:
    ConnectionHandleIO() {
        // The handle_read/handle_write must have a 'ErrorHandler'
        handlers["ErrorHandler"] = std::unique_ptr<RequestHandler>(new TestHandler1());
    }

    bool HandleReadPartial(const boost::system::error_code& ec)
    {
        boost::asio::ip::tcp::socket socket(io_service);
        std::shared_ptr<Connection> conn = std::make_shared<Connection>(std::move(socket), handlers);
        return conn->handle_read_partial(ec, 0);
    }

    bool HandleReadBody(const boost::system::error_code& ec)
    {
        boost::asio::ip::tcp::socket socket(io_service);
        std::shared_ptr<Connection> conn = std::make_shared<Connection>(std::move(socket), handlers);
        return conn->handle_read_body(ec, 0);
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

TEST_F(ConnectionHandleIO, HandleReadPartial) {
    boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);
    EXPECT_TRUE(HandleReadPartial(ec_success)); 

    boost::system::error_code ec_broken_pipeline = boost::system::errc::make_error_code(boost::system::errc::broken_pipe);
    EXPECT_FALSE(HandleReadPartial(ec_broken_pipeline));    

    boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
    EXPECT_FALSE(HandleReadPartial(ec_connection_aborted));   
}

TEST_F(ConnectionHandleIO, HandleReadBody) {
    boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);
    EXPECT_TRUE(HandleReadBody(ec_success)); 

    boost::system::error_code ec_broken_pipeline = boost::system::errc::make_error_code(boost::system::errc::broken_pipe);
    EXPECT_FALSE(HandleReadBody(ec_broken_pipeline));    

    boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
    EXPECT_FALSE(HandleReadBody(ec_connection_aborted));   
}

TEST_F(ConnectionHandleIO, HandleWrite) {
    boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);
    EXPECT_TRUE(HandleWrite(ec_success)); 

    boost::system::error_code ec_broken_pipeline = boost::system::errc::make_error_code(boost::system::errc::broken_pipe);
    EXPECT_FALSE(HandleWrite(ec_broken_pipeline));    

    boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
    EXPECT_FALSE(HandleWrite(ec_connection_aborted));   
}

} // namespace server
} // namespace http







