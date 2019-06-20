#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include "server/http/HttpRequest.h"
#include "server/http/HttpResponse.h"

namespace http{

class HttpMessage
{
public:
    using addr_t = net::inet_addr;
    using string_t = util::string;
public:
    HttpMessage(/* args */);
    ~HttpMessage();

private:
    addr_t          dstAddress_;
    string_t        dstIP_; 
    string_t        dstPort_;

    string_t        localIP_;
    string_t        versionStr_;
};

}//namespace http

#endif //HTTP_MESSAGE_H_
