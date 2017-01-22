#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <string>

namespace http {
namespace server {

class connection
	: public std::enable_shared_from_this<connection>
{
public:
	connection(const connection&) = delete;
  	connection& operator=(const connection&) = delete;

  	explicit connection(boost::asio::ip::tcp::socket socket);

  	void start();

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