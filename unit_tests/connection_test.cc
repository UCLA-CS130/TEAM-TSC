#include "gtest/gtest.h"
#include <boost/asio.hpp>
#include "../src/connection.h"

class ConnectionTest: public ::testing::Test {
protected:
	//ConnectionTest(): socket_(io_service) {}

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

/*TEST(Connection, ReadAndWrite)
{
	boost::asio::io_service io_service1, io_service2;

	boost::asio::ip::tcp::resolver resolver(io_service1);
	//boost::asio::ip::tcp::endpoint receiver_endpoint = *resolver.resolve({"0.0.0.0", "8080"});
   	//boost::asio::connect(s, resolver.resolve({argv[1], argv[2]}));
	boost::asio::ip::tcp::endpoint receiver_endpoint(boost::asio::ip::address::from_string("0.0.0.0"), 8088);
	boost::asio::ip::tcp::endpoint sender_endpoint(boost::asio::ip::address::from_string("0.0.0.0"), 8089);

	boost::system::error_code ec;
	boost::asio::ip::tcp::socket receiver(io_service1);
	receiver.open(receiver_endpoint.protocol());
	receiver.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	receiver.bind(receiver_endpoint, ec);
	if (ec)
	{
		std::cout << "errors occured" << std::endl;//An error occurred.
	}

	boost::asio::ip::tcp::socket sender(io_service2);
	sender.open(sender_endpoint.protocol());
	sender.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	sender.bind(sender_endpoint, ec);
	if (ec)
	{
		std::cout << "errors occured" << std::endl;//An error occurred.
	}	

	sender.connect(receiver_endpoint, ec);
	std::make_shared<http::server::Connection>(std::move(receiver))->start();
	io_service1.run();


    std::string request = "hello from 8081!\r\n\r\n";
    //boost::asio::write(sender, boost::asio::buffer(request));
    sender.write(boost::asio::buffer(request))

    std::array<char, 100> reply;
    size_t reply_length = boost::asio::read(sender,
        boost::asio::buffer(reply, 100));


	std::string receive_string = std::string(reply.data(), reply.data() + reply_length);
	std::string return_string = "HTTP/1.0 200 OK\r\nContent-Length: 20\r\nontent-Type: text/plain\r\n\r\n" + request;

	std::cout << receive_string << std::endl;
	EXPECT_EQ(receive_string, return_string);
	//EXPECT_EQ(reply_content, "hello from 8081!");
	//EXPECT_STREQ(receive_data, "HTTP/1.0 200 OK\r\nContent-Length: 20\r\nontent-Type: text/plain\r\n\r\n" + send_data);
}*/





