#include <utility>
#include <vector>
#include "connection.h"

namespace http {
namespace server {

Connection::Connection(boost::asio::ip::tcp::socket socket, 
                       std::map<std::string, std::unique_ptr<RequestHandler>>& handlers_)
	: socket_(std::move(socket)),
    handlers(handlers_)
{
}

void Connection::start() 
{
	do_read_partial();
}

void Connection::do_read_partial() 
{

  boost::asio::async_read_until(socket_, buffer_, "\r\n\r\n",
                                boost::bind(&Connection::handle_read_partial, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
}


void Connection::do_read_body(std::size_t left_content_length) 
{
    // The largest request is 8192, much smaller than streambuf.max_size()
    boost::asio::async_read(socket_, buffer_, boost::asio::transfer_exactly(left_content_length),
                            boost::bind(&Connection::handle_read_body, shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}


bool Connection::handle_read_partial(const boost::system::error_code& ec, 
                                     size_t bytes_transferred) 
{
  if (!ec) {
    // consume the streambuf
    std::ostringstream ss;
    ss << &buffer_;
    raw_request = ss.str();

    std::unique_ptr<Request> request_ptr = Request::Parse(raw_request);
    if (!request_ptr) {
      response.SetStatus(Response::bad_request);
      handlers["ErrorHandler"]->HandleRequest(request, &response);
    }
    else {
      request = *request_ptr;
      if (request.body().length() < request.content_length()) {
        //then we should go on reading the request body
        do_read_body(request.content_length() - request.body().length());
        return true;
      }
      else if (!ProcessRequest(request.uri())) {
        handlers["ErrorHandler"]->HandleRequest(request, &response);
      }
    }
    do_write();
    return true;
  }
  else if (ec != boost::asio::error::operation_aborted) {
    socket_.close();
  }
  return false;
}

bool Connection::handle_read_body(const boost::system::error_code& ec, 
                                  size_t bytes_transferred) 
{
  if (!ec) {
     // consume the streambuf
    std::ostringstream ss;
    ss << &buffer_;
    std::string left_request = ss.str();

    request.AppendBody(left_request);
    if (!ProcessRequest(request.uri())) {
      handlers["ErrorHandler"]->HandleRequest(request, &response);
    }
    do_write();
    return true;
  }
  else if (ec != boost::asio::error::operation_aborted) {
    socket_.close();
  }
  return false;
}

bool
Connection::ProcessRequest(const std::string& uri) 
{
  std::size_t pos = 1;
  std::string longest_prefix = "";
  while (true) {
    std::size_t found = uri.find("/", pos);
    auto it = handlers.find(uri.substr(0, found));
    if (it != handlers.end()) longest_prefix = it->first;
    if (found != std::string::npos) pos = found + 1;
    else break;
  }
  if (longest_prefix == "") {
    BOOST_LOG_TRIVIAL(info) << "No matched handler for request prefix";
    response.SetStatus(Response::bad_request);
    return false;
  }
  
  RequestHandler::Status status = handlers[longest_prefix]->HandleRequest(request, &response);
  if (status != RequestHandler::ok) return false;
  return true;
}

void 
Connection::do_write() {
  ServerStatus::getInstance().addStatusCodeAndTotalVisit(response.GetStatus());
	boost::asio::async_write(socket_, boost::asio::buffer(response.ToString()),
      boost::bind(&Connection::handle_write, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

bool 
Connection::handle_write(const boost::system::error_code& ec, std::size_t) {
  bool none_ec = false;
  if (!ec) {
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    none_ec = true;
  }
  if (ec != boost::asio::error::operation_aborted) {
    socket_.close();
  }
  return none_ec;
}

} // namespace server
} // namespace http
