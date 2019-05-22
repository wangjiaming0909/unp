#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "server/net/inet_addr.h"
#include "server/util/XString.h"
#include "server/http/http_method.h"
#include <variant>

namespace http
{
struct http_request
{
    using addr_t = net::inet_addr;
    using string_t = util::string;
    addr_t      clientAddr_; 
    string_t    clientIP_;
    string_t    clientPort_;
    std::variant<string_t, http_method> method_;
    string_t    path_;
    string_t    query_;
    string_t    utl_;
    uint16_t    pushStatus_;
    string_t    pushStatusStr_;
};
}
#endif //_HTTP_REQUEST_H_