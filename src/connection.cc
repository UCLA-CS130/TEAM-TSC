#include "connection.h"
#include <utility>
#include <vector>


namespace http {
namespace server {

const std::string ok =
  "HTTP/1.0 200 OK\r\n";

connection::connection(boost::asio::ip::tcp::socket socket)
	: socket_(std::move(socket))
{
}

void connection::start() {
	do_read();
}

void connection::do_read() {
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(buffer_),
      [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
          reply_content.append(buffer_.data(), buffer_.data() + bytes_transferred);
	  //TODO: Fix bug: Error if client sends string with size < 3
          if(reply_content.substr(reply_content.size() - 4, 4) == "\r\n\r\n") {
          		do_write();
          }
          else do_read();
        }
      });
}

void connection::do_write() {
	auto self(shared_from_this());

  //Browser display strange things here if multiple push_back operations on buffer are used. 
  std::string data_to_send =   ok
                          + "Content-Length: "
                          + std::to_string(reply_content.size())
                          + "\r\nContent-Type: text/plain\r\n\r\n"
                          + reply_content;



	boost::asio::async_write(socket_, boost::asio::buffer(data_to_send),
      [this, self](boost::system::error_code ec, std::size_t)
      {
        if (!ec) {
          // Initiate graceful connection closure.
          boost::system::error_code ignored_ec;
          socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
            ignored_ec);
        }
      });
}


}
}
