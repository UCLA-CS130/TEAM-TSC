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

  void SetVersion(const std::string& version) {
    version_ = version;
  }
  
  void SetStatus(const ResponseCode& response_code) {
    status_ = response_code;
  }

  void SetStatus(const unsigned int response_code);

  void AddHeader(std::string header_name, std::string header_value);


  std::string GetBody(){
    return body_;
  }

  void SetBody(const std::string& body) {
    body_ = body;
  }

  void AppendBody(const std::string& body) {
    body_.append(body);
  }

  void Clear() {
    headers_.clear();
    body_.clear();
  }
  
  ResponseCode GetStatus() {
    return status_;
  }

  std::string ToString();

  void setIsImage(bool is_image){
    is_image_ = is_image;
  }

  bool getIsImage(){
    return is_image_;
  }

 private:
  std::string version_;
  ResponseCode status_;
  Headers headers_;
  std::string body_;
  bool is_image_;
};

} // namespace server
} // namespace http

#endif // HTTP_RESPONSE_H