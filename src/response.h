#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

namespace http {
namespace server {

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
    internal_server_error = 500,
    redirect = 302
  };

  using Headers = std::vector<std::pair<std::string, std::string>>;
  
  void SetStatus(const ResponseCode& response_code) {
    status_ = response_code;
  }

  void AddHeader(std::string header_name, std::string header_value) {
    headers_.push_back(std::make_pair(header_name, header_value));
  }

  void SetBody(const std::string& body) {
    body_ = body;
  }

  void AppendBody(const std::string& body) {
    body_.append(body);
  }
  
  ResponseCode GetStatus() {
    return status_;
  }

  std::string ToString();

 private:
  ResponseCode status_;
  Headers headers_;
  std::string body_;
};

} // namespace server
} // namespace http

#endif // HTTP_RESPONSE_H