#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

namespace http{
namespace server {

// For the Request and Response classes, you need to implement the methods
// and add private data as appropriate. You may also need to modify or extend
// the API when implementing the reverse proxy. Use your good judgment.

// Represents an HTTP Request.
//
// Usage:
//   auto request = Request::Parse(raw_request);
class Request {
 public:

  Request():content_length_(0) {}

  static std::unique_ptr<Request> Parse(const std::string& raw_request);

  using Headers = std::vector<std::pair<std::string, std::string>>;

  std::string ToString() const;

  std::string raw_request() const {
    return raw_request_;
  }

  std::string method() const { 
    return method_; 
  }

  std::string uri() const { 
    return uri_; 
  }

  std::string version() const { 
    return version_; 
  }

  Headers headers() const { 
    return headers_; 
  }

  std::string body() const { 
    return body_; 
  }

  std::size_t content_length() const{
    return content_length_;
  }

  void SetRawRequest(const std::string& raw_request) {
    raw_request_ = raw_request;
  }

  void SetMethod(const std::string& method) { 
    method_ = method; 
  }

  void SetUri(const std::string& uri) { 
    uri_ = uri; 
  }

  void SetVersion(const std::string& version) { 
    version_ = version; 
  }

  void AddHeader(std::string header_name, std::string header_value) {
    headers_.push_back(std::make_pair(header_name, header_value));
  }

  void SetBody(const std::string& body) { 
    body_ = body; 
  }

  void SetContentLength(const std::size_t content_length) {
    content_length_ = content_length;
  }

  void AppendBody(const std::string left_body) {
    body_.append(left_body);
    raw_request_.append(left_body);
  }

 private:
  std::string raw_request_;
  std::string method_;
  std::string uri_;
  std::string version_;
  Headers headers_;
  std::string body_;
  std::size_t content_length_;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_H