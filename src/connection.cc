#include "connection.h"
#include <utility>
#include <vector>


namespace http {
namespace server {

Connection::Connection(boost::asio::ip::tcp::socket socket, 
                       RequestHandler& echo_request_handler_, 
                       RequestHandler& static_request_handler_,
                       RequestParserInterface* request_parser_)
	: socket_(std::move(socket)),
    echo_request_handler(echo_request_handler_),
    static_request_handler(static_request_handler_),
    request_parser(request_parser_)
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
    RequestParserInterface::result_type result;
    request_string.append(buffer_.data(), buffer_.data() + bytes_transferred);
    std::tie(result, std::ignore) = request_parser->parse(
              request_, buffer_.data(), buffer_.data() + bytes_transferred);

    if (result == RequestParserInterface::good) {
      BOOST_LOG_TRIVIAL(trace) << "Connection: RequestParser returns good;\n";
      std::string uri = request_.uri;
      if (echo_request_handler.check_serve_path(uri)) {
        echo_request_handler.handle_request(request_string, request_, reply_);
      }
      else if (static_request_handler.check_serve_path(uri)) {
        static_request_handler.handle_request(request_string, request_, reply_);
      }
      else reply_ = reply::stock_reply(reply::bad_request);
      do_write();
    }
    else if (result == RequestParserInterface::bad) {
      BOOST_LOG_TRIVIAL(trace) << "Connection: RequestParser returns bad;\n";
      reply_ = reply::stock_reply(reply::bad_request);
      do_write();
    }
    else {
      BOOST_LOG_TRIVIAL(trace) << "Connection: RequestParser returns intermediate;\n";
      do_read();
    }
    return true;
  }
  else
    return false;
}

void 
Connection::do_write() {
	boost::asio::async_write(socket_, reply_.to_buffers(),
      boost::bind(&Connection::handle_write, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

bool 
Connection::handle_write(const boost::system::error_code& ec, std::size_t) {
  if (!ec) {
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    return true;
  }
  else return false;
}


}
}
