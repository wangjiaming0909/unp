#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include "server/http/HttpRequest.h"
#include "server/http/HttpResponse.h"
#include "server/http/HttpHeaders.h"
#include "server/util/unp_time.h"
#include <variant>

namespace http{

class HttpMessage
{
public:
    using addr_t = net::inet_addr;
    using string_t = util::string;
public:
    HttpMessage();
    ~HttpMessage();

private:
    std::variant<std::monostate, HttpRequest, HttpResponse> message_;

    HttpHeaders     headers_;
    util::Time      startTime_;

    addr_t          dstAddress_;
    string_t        dstIP_; 
    string_t        dstPort_;
    string_t        localIP_;
    string_t        versionStr_;
};

}//namespace http

#endif //HTTP_MESSAGE_H_
