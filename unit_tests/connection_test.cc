#include "gtest/gtest.h"
#include <boost/asio.hpp>
#include "../src/connection.h"

class ConnectionTest: public ::testing::Test {
protected:
    bool handle_read(boost::system::error_code ec) {
    	boost::asio::ip::tcp::socket socket_(io_service);
        return std::make_shared<http::server::Connection>(std::move(socket_))->handle_read(ec, bytes_transferred);
    }
    bool handle_write(boost::system::error_code ec) {
        boost::asio::ip::tcp::socket socket_(io_service);
        return std::make_shared<http::server::Connection>(std::move(socket_))->handle_write(ec, bytes_transferred);
    }
    size_t bytes_transferred = 32;
    boost::asio::io_service io_service;
};

TEST_F(ConnectionTest, ReadHandler) {
	boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);
    EXPECT_TRUE(handle_read(ec_success));

	boost::system::error_code ec_broken_pipeline = boost::system::errc::make_error_code(boost::system::errc::broken_pipe);
    EXPECT_FALSE(handle_read(ec_broken_pipeline));    

	boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
    EXPECT_FALSE(handle_read(ec_connection_aborted));      
}

TEST_F(ConnectionTest, WriteHandler) {
	boost::system::error_code ec_success = boost::system::errc::make_error_code(boost::system::errc::success);
    EXPECT_TRUE(handle_write(ec_success));

	boost::system::error_code ec_broken_pipeline = boost::system::errc::make_error_code(boost::system::errc::broken_pipe);
    EXPECT_FALSE(handle_write(ec_broken_pipeline));    

	boost::system::error_code ec_connection_aborted = boost::system::errc::make_error_code(boost::system::errc::connection_aborted);
    EXPECT_FALSE(handle_write(ec_connection_aborted));   
}






