#include "connection.hpp"
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
  std::cout << "start to read!" << std::endl;
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(buffer_),
      [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
      {
        if (!ec)
        {
          reply_content.append(buffer_.data(), buffer_.data() + bytes_transferred);
          if(reply_content.substr(reply_content.size() - 4, 4) == "\r\n\r\n") {
          		do_write();
          }
          else do_read();
        }
      });
}

void connection::do_write() {
	auto self(shared_from_this());
	std::vector<boost::asio::const_buffer> from_buffers;
	from_buffers.push_back(boost::asio::buffer(ok));

	from_buffers.push_back(boost::asio::buffer("Content-Length: "));
	from_buffers.push_back(boost::asio::buffer(std::to_string(reply_content.size())));
	from_buffers.push_back(boost::asio::buffer("\r\n"));

	from_buffers.push_back(boost::asio::buffer("Content-Type: text/plain\r\n"));

	from_buffers.push_back(boost::asio::buffer(reply_content));

	boost::asio::async_write(socket_, from_buffers,
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