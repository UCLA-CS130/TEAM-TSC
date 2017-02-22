#ifndef HTTP_REQUEST_HANDLER_H
#define HTTP_REQUEST_HANDLER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "config_parser.h"

namespace http {
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
  static std::unique_ptr<Request> Parse(const std::string& raw_request);

  using Headers = std::vector<std::pair<std::string, std::string>>;
  std::string raw_request() const { return raw_request_; }
  std::string method() const { return method_; }
  std::string uri() const { return uri_; }
  std::string version() const { return version_; }
  Headers headers() const { return headers_; }
  std::string body() const { return body_; }

  void SetRawRequest(const std::string& raw_request) { raw_request_ = raw_request; }
  void SetMethod(const std::string& method) { method_ = method; }
  void SetUri(const std::string& uri) { uri_ = uri; }
  void SetVersion(const std::string& version) { version_ = version; }

  void AddHeader(std::string header_name, std::string header_value)
  {
    headers_.push_back(std::make_pair(header_name, header_value));
  }
  void SetBody(const std::string& body) { body_ = body; }

 private:
  std::string raw_request_;
  std::string method_;
  std::string uri_;
  std::string version_;
  Headers headers_;
  std::string body_;
  
};

// Represents an HTTP response.
//
// Usage:
//   Response r;
//   r.SetStatus(RESPONSE_200);
//   r.SetBody(...);
//   return r.ToString();
//
// Constructed by the RequestHandler, after which the server should call ToString
// to serialize.
class Response {
 public:
  enum ResponseCode {
    // Define your HTTP response codes here.
    ok = 200,
    bad_request = 400,
    not_found = 404,
    internal_server_error = 500
  };

  using Headers = std::vector<std::pair<std::string, std::string>>;
  
  void SetStatus(const ResponseCode& response_code) 
  {
    status = response_code;
  }

  bool IsStatus(const ResponseCode& response_code)
  {
    return (response_code == status);
  }

  void AddHeader(std::string header_name, std::string header_value)
  {
    headers_.push_back(std::make_pair(header_name, header_value));
  }

  void SetBody(const std::string& body)
  {
    body_ = body;
  }
  
  ResponseCode GetStatus() {
    return status;
  }

  std::string ToString();

 private:
  ResponseCode status;
  Headers headers_;
  std::string body_;
};

// Represents the parent of all request handlers. Implementations should expect to
// be long lived and created at server constrution.
class RequestHandler {
 public:
  enum Status {
    ok = 0,
    handle_fail = 1,
    invalid_root_format = 2
  };

  static RequestHandler* CreateByName(const char* type);
  
  // Initializes the handler. Returns true if successful.
  // uri_prefix is the value in the config file that this handler will run for.
  // config is the contents of the child block for this handler ONLY.
  virtual Status Init(const std::string& uri_prefix,
                      const NginxConfig& config) = 0;

  // Handles an HTTP request, and generates a response. Returns a response code
  // indicating success or failure condition. If ResponseCode is not OK, the
  // contents of the response object are undefined, and the server will return
  // HTTP code 500.
  virtual Status HandleRequest(const Request& request,
                               Response* response) = 0;
};

extern std::map<std::string, RequestHandler* (*)(void)>* request_handler_builders;
template<typename T>
class RequestHandlerRegisterer {
 public:
  RequestHandlerRegisterer(const std::string& type) {
    if (request_handler_builders == nullptr) {
      request_handler_builders = new std::map<std::string, RequestHandler* (*)(void)>;
    }
    (*request_handler_builders)[type] = RequestHandlerRegisterer::Create;
  }
  static RequestHandler* Create() {
    return new T;
  }
};

#define REGISTER_REQUEST_HANDLER(ClassName) \
  static RequestHandlerRegisterer<ClassName> ClassName##__registerer(#ClassName)

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP
