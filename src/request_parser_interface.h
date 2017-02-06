#ifndef HTTP_REQUEST_PARSER_INTERFACE_H
#define HTTP_REQUEST_PARSER_INTERFACE_H

#include <tuple>
#include "request.h"

namespace http {
namespace server {

class RequestParserInterface {
 public:
   virtual ~RequestParserInterface() {};

   enum result_type { good, bad, indeterminate };

   virtual 
   std::tuple<result_type, char*> parse(request& req, char* begin, char* end) = 0;
};

} // server
} // http

#endif // HTTP_REQUEST_PARSER_INTERFACE_H