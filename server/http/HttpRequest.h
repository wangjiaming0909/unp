#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "server/net/inet_addr.h"
#include "server/http/HttpMethod.h"
#include <boost/variant/variant.hpp> 
#include <string>

namespace http
{
struct HttpRequest
{
    using addr_t = net::inet_addr;
    using string_t = std::string;

    addr_t                                  clientAddr_; 
    string_t                                clientIP_;
    string_t                                clientPort_;
    string_t                                path_;
    string_t                                query_;
    string_t                                utl_;
    uint16_t                                pushStatus_;
    string_t                                pushStatusStr_;
    boost::variant<string_t, HTTPMethod>      method_;
};
}
#endif //_HTTP_REQUEST_H_