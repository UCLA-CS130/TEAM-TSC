#include <utility>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
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
      std::size_t content_length;
      std::string content_length_str = request.GetHeaderValueByName("Content-Length");
      if (content_length_str.empty()) content_length = 0;
      else content_length = boost::lexical_cast<std::size_t>(content_length_str);
      if (request.body().length() < content_length) {
        //then we should go on reading the request body
        do_read_body(content_length - request.body().length());
        return true;
      }
      else if (!ProcessRequest(request)) {
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
    if (!ProcessRequest(request)) {
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
Connection::ProcessRequest(const Request& request) 
{
  std::string longest_prefix = request.uri();
  while(!longest_prefix.empty()) {
    auto it = handlers.find(longest_prefix);
    if (it != handlers.end()) break;
    std::size_t pos = longest_prefix.find_last_of("/");
    longest_prefix = longest_prefix.substr(0, pos);
  } 
  if (longest_prefix == "") {
    std::string referer = request.GetHeaderValueByName("Referer");
    std::string host = request.GetHeaderValueByName("Host");
    // if is a followed proxy request
    if (referer != "" && 
        referer.find(host) != std::string::npos &&
        handlers.find("/") != handlers.end()) {
        //auto ref_uri = referer.find("/",8);
        //longest_prefix = referer.substr(ref_uri);
        longest_prefix = "/";
    }
  }

  if (longest_prefix == "") {
    BOOST_LOG_TRIVIAL(info) << "No matched handler for request prefix";
    response.SetStatus(Response::bad_request);
    return false;
  }
  
  RequestHandler::Status status;
  status = handlers[longest_prefix]->HandleRequest(request, &response);
  if (status != RequestHandler::ok) return false;
  return true;
}



void
Connection::compress_payload()
{
  std::string encoding_string = request.GetHeaderValueByName("Accept-Encoding");
  if(encoding_string!=""){
    //then there is a accepted compression format
    std::vector<std::string> encode_format;
    boost::replace_all(encoding_string," ","");
    boost::split(encode_format,encoding_string,boost::is_any_of(","));
   
    Compression com(response.GetBody());

    if(find(encode_format.begin(),encode_format.end(),"deflate") != encode_format.end()){
      response.AddHeader("Content-Encoding","deflate");
      std::cout<<"Compressing using deflate"<<std::endl;
      com.CompressDeflate();
      response.SetBody(com.GetCompressedBody());

    } else if(find(encode_format.begin(),encode_format.end(),"gzip") != encode_format.end()){
      response.AddHeader("Content-Encoding","gzip");
      std::cout<<"Compressing using gzip"<<std::endl;
      com.CompressGzip();
      response.SetBody(com.GetCompressedBody());
    }
  }
}



void 
Connection::do_write() {
  ServerStatus::getInstance().insertRecord(request.uri(), response.GetStatus());
  compress_payload();
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
