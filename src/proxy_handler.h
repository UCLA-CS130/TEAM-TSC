#ifndef HTTP_PROXY_HANDLER_H
#define HTTP_PROXY_HANDLER_H

#include <string>
#include <boost/asio.hpp>
#include "request_handler.h"
#include "server_status.h"
#include "request.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unordered_map>

namespace http {
namespace server {
  
/// The common handler for all incoming requests.
class ProxyHandler: public RequestHandler
{
 public:
  ProxyHandler();

  RequestHandler::Status Init(const std::string& uri_prefix,
                              const NginxConfig& config);

  RequestHandler::Status HandleRequest(const Request& request,
                                       Response* response);

  RequestHandler::Status HandleError(const std::string& error_info, 
                                     const boost::system::error_code& ec);

  std::string MakeRequest(const Request& request,
                          const std::string& uri,
                          const std::string& server_host,
                          const std::string& server_port);

  void ParseLocation(const std::string& location_header,
                     std::string& server,
                     std::string& port,
                     std::string& path);

  void MakeCache(const std::string& uri, 
                 boost::posix_time::ptime expiration,
                 std::string e_tag,
                 Response *response);

  bool HandleCache(const std::string& uri,
                   Response *response);

  bool UpdateCache(const std::string& uri, const std::string& path);

 private:

  struct cache_sign {
  public:
    boost::posix_time::ptime expiration_;
    std::string e_tag_;

    cache_sign() {};
    
    cache_sign(const boost::posix_time::ptime& expiration,
               const std::string& e_tag) 
    {
      expiration_ = expiration;
      e_tag_ = e_tag;
    }
  };

  boost::asio::io_service io_service_;

  std::string uri_prefix_;

  std::string server_host_;

  std::string server_port_;

  std::unordered_map<std::string, cache_sign> cache_;
};

REGISTER_REQUEST_HANDLER(ProxyHandler);

} // server
} // http

#endif // HTTP_PROXY_HANDLER_H
