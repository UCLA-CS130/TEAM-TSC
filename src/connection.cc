#include <utility>
#include <vector>
#include <boost/log/trivial.hpp>
#include "connection.h"



namespace http {
namespace server {

Connection::Connection(boost::asio::ip::tcp::socket socket, 
                       std::map<std::string, std::unique_ptr<RequestHandler>>& handlers_)
	: socket_(std::move(socket)),
    handlers(handlers_)
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
    std::string raw_request = "";
    raw_request.append(buffer_.data(), buffer_.data() + bytes_transferred);
    std::unique_ptr<Request> request_ptr = Request::Parse(raw_request);
    if (!request_ptr) {
      response.SetStatus(Response::bad_request);
      //handlers["ErrorHandler"]->HandleRequest(*request, response.get());
    }
    else {
      request = *request_ptr;
      if (!ProcessRequest(request.uri())) {
        //handlers["ErrorHandler"]->HandleRequest(*request, response.get());
      }
    }
    do_write();
    return true;
  }
  else
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
    ServerStatus::getInstance().addRecord(uri,(ResponseCode)Response::bad_request);
    return false;
  }
  
  RequestHandler::Status status = handlers[longest_prefix]->HandleRequest(request, &response);
  ServerStatus::getInstance().addRecord(uri,(ResponseCode)response.GetStatus());
  if (status != RequestHandler::ok) return false;
  return true;
}

void 
Connection::do_write() {
	boost::asio::async_write(socket_, boost::asio::buffer(response.ToString()),
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
