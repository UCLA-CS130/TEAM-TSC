#include <boost/asio.hpp>
#include "../src/connection.h"
#include "request_handler_mock.h"
#include "../src/reply.h"

// for gmock testing
namespace http {
namespace server {
  bool operator==(const reply& a, const reply& b) {
    return true;
  };
}
}

using namespace http::server;
using ::testing::_;
using ::testing::Return;


// request_parser returns good --> echo_request_handler 
TEST(ConnectionTest, EchoHandler) {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket_(io_service);

	boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);

    char* ignore = new char;
    std::tuple<RequestParser::result_type, char*> request_parser_return = std::make_tuple(RequestParser::good, ignore);
    reply rep;

    RequestParserMock *request_parser_mock = new RequestParserMock();
    RequestHandlerMock echo_request_handler_mock;
    RequestHandlerMock static_request_handler_mock;

    EXPECT_CALL(*request_parser_mock, parse(_, _, _))\
            .Times(::testing::Exactly(1))\
            .WillOnce(::testing::Return(request_parser_return));
    EXPECT_CALL(echo_request_handler_mock, check_serve_path(_))\
            .Times(::testing::Exactly(1))\
            .WillOnce(::testing::Return(true));
    EXPECT_CALL(echo_request_handler_mock, handle_request(_, _, rep))\
            .Times(::testing::Exactly(1));

    bool connection_return = std::make_shared<Connection>(std::move(socket_), 
                                                          echo_request_handler_mock,
                                                          static_request_handler_mock,
                                                          request_parser_mock)->handle_read(ec_success, 0);
    EXPECT_TRUE(connection_return);  
    delete request_parser_mock; 
}

// request_parser returns good --> static_request_handler 
TEST(ConnectionTest, StaticHandler) {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket_(io_service);

    boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);

    char* ignore = new char;
    std::tuple<RequestParser::result_type, char*> request_parser_return = std::make_tuple(RequestParser::good, ignore);
    reply rep;

    RequestParserMock *request_parser_mock = new RequestParserMock();
    RequestHandlerMock echo_request_handler_mock;
    RequestHandlerMock static_request_handler_mock;

    EXPECT_CALL(*request_parser_mock, parse(_, _, _))\
            .Times(::testing::Exactly(1))\
            .WillOnce(::testing::Return(request_parser_return));
    EXPECT_CALL(echo_request_handler_mock, check_serve_path(_))\
            .Times(::testing::Exactly(1))\
            .WillOnce(::testing::Return(false));
    EXPECT_CALL(static_request_handler_mock, check_serve_path(_))\
            .Times(::testing::Exactly(1))\
            .WillOnce(::testing::Return(true));
    EXPECT_CALL(static_request_handler_mock, handle_request(_, _, rep))\
            .Times(::testing::Exactly(1));

    bool connection_return = std::make_shared<Connection>(std::move(socket_), 
                                                          echo_request_handler_mock,
                                                          static_request_handler_mock,
                                                          request_parser_mock)->handle_read(ec_success, 0);
    EXPECT_TRUE(connection_return); 
    delete request_parser_mock;   
}

// request_parser returns good --> non-suitable-handler
TEST(ConnectionTest, NoneHandler) {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket_(io_service);

    boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);

    char* ignore = new char;
    std::tuple<RequestParser::result_type, char*> request_parser_return = std::make_tuple(RequestParser::good, ignore);
    reply rep;

    RequestParserMock *request_parser_mock = new RequestParserMock();
    RequestHandlerMock echo_request_handler_mock;
    RequestHandlerMock static_request_handler_mock;

    EXPECT_CALL(*request_parser_mock, parse(_, _, _))\
            .Times(::testing::Exactly(1))\
            .WillOnce(::testing::Return(request_parser_return));
    EXPECT_CALL(echo_request_handler_mock, check_serve_path(_))\
            .Times(::testing::Exactly(1))\
            .WillOnce(::testing::Return(false));
    EXPECT_CALL(static_request_handler_mock, check_serve_path(_))\
            .Times(::testing::Exactly(1))\
            .WillOnce(::testing::Return(false));

    bool connection_return = std::make_shared<Connection>(std::move(socket_), 
                                                          echo_request_handler_mock,
                                                          static_request_handler_mock,
                                                          request_parser_mock)->handle_read(ec_success, 0);
    EXPECT_TRUE(connection_return);
    delete request_parser_mock;    
}

// request_parser returns false
TEST(ConnectionTest, RequestParserFail) {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket_(io_service);

    boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);

    char* ignore = new char;
    std::tuple<RequestParser::result_type, char*> request_parser_return = std::make_tuple(RequestParser::bad, ignore);
    reply rep;

    RequestParserMock *request_parser_mock = new RequestParserMock();
    RequestHandlerMock echo_request_handler_mock;
    RequestHandlerMock static_request_handler_mock;

    EXPECT_CALL(*request_parser_mock, parse(_, _, _))\
            .Times(::testing::Exactly(1))\
            .WillOnce(::testing::Return(request_parser_return));

    bool connection_return = std::make_shared<Connection>(std::move(socket_), 
                                                          echo_request_handler_mock,
                                                          static_request_handler_mock,
                                                          request_parser_mock)->handle_read(ec_success, 0);
    EXPECT_TRUE(connection_return);   
    delete request_parser_mock;
}

class DealWithErrorCode: public::testing::Test{
protected:
    bool handle_read(boost::system::error_code ec) {
        boost::asio::ip::tcp::socket socket_(io_service);
        request_parser_mock = new RequestParserMock();
        return std::make_shared<Connection>(std::move(socket_), 
                                            echo_request_handler_mock,
                                            static_request_handler_mock,
                                            request_parser_mock)->handle_read(ec, bytes_transferred);
    }
    
    bool handle_write(boost::system::error_code ec) {
        boost::asio::ip::tcp::socket socket_(io_service);
        request_parser_mock = new RequestParserMock();
        return std::make_shared<Connection>(std::move(socket_), 
                                            echo_request_handler_mock,
                                            static_request_handler_mock,
                                            request_parser_mock)->handle_write(ec, bytes_transferred);
    }

    size_t bytes_transferred = 32;
    boost::asio::io_service io_service;
    RequestParserMock *request_parser_mock;
    RequestHandlerMock echo_request_handler_mock;
    RequestHandlerMock static_request_handler_mock;
};

TEST_F(DealWithErrorCode, ReadHandler) {
	boost::system::error_code ec_broken_pipeline = boost::system::errc::make_error_code(boost::system::errc::broken_pipe);
    EXPECT_FALSE(handle_read(ec_broken_pipeline));    

	boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
    EXPECT_FALSE(handle_read(ec_connection_aborted));   
}

TEST_F(DealWithErrorCode, WriteHandler) {
    boost::system::error_code ec_broken_pipeline = boost::system::errc::make_error_code(boost::system::errc::broken_pipe);
    EXPECT_FALSE(handle_write(ec_broken_pipeline));    

    boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
    EXPECT_FALSE(handle_write(ec_connection_aborted));   
}





