#include "connection.h"
#include <utility>
#include <vector>


namespace http {
namespace server {

const std::string ok =
  "HTTP/1.0 200 OK\r\n";

Connection::Connection(boost::asio::ip::tcp::socket socket)
	: socket_(std::move(socket))
{
}

void Connection::start() {
	do_read();
}

void Connection::do_read() {
	socket_.async_read_some(boost::asio::buffer(buffer_),
      boost::bind(&Connection::handle_read, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

bool Connection::handle_read(const boost::system::error_code& ec, 
                             size_t bytes_transferred) {
  if (!ec) {
    reply_content.append(buffer_.data(), buffer_.data() + bytes_transferred);
    //TODO: Fix bug: Error if client sends string with size < 3
    if(reply_content.substr(reply_content.size() - 4, 4) == "\r\n\r\n") {
      do_write();
      return true;
    }
    else {
      do_read();
      return true;
    }
  }
  else {
    return false;
  }
}

void Connection::do_write() {
  std::string data_to_send =   ok
                          + "Content-Length: "
                          + std::to_string(reply_content.size())
                          + "\r\nContent-Type: text/plain\r\n\r\n"
                          + reply_content;

	boost::asio::async_write(socket_, boost::asio::buffer(data_to_send),
      boost::bind(&Connection::handle_write, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

bool Connection::handle_write(const boost::system::error_code& ec, std::size_t) {
  if (!ec) {
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    return true;
  }
  else return false;
}


}
}
