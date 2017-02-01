#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <string>
#include <boost/bind.hpp>

namespace http {
namespace server {

class Connection
	: public std::enable_shared_from_this<Connection>
{
public:
	Connection(const Connection&) = delete;
  	Connection& operator=(const Connection&) = delete;

  	explicit Connection(boost::asio::ip::tcp::socket socket);

  	void start();

  	bool handle_read(const boost::system::error_code& ec, 
                     std::size_t bytes_transferred);

  	bool handle_write(const boost::system::error_code& ec,
  					  std::size_t);

private:
	void do_read();

	void do_write();

	boost::asio::ip::tcp::socket socket_;

	std::array<char, 8192> buffer_;

	std::string reply_content;

};

} // namespace server
} // namespace http

#endif // HTTP_CONNECTION_HPP