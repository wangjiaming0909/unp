#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include "server/http/http_request.h"
#include "server/http/http_response.h"

namespace http{

class http_message
{
public:
    using addr_t = net::inet_addr;
    using string_t = util::string;
public:
    http_message(/* args */);
    ~http_message();

private:
    addr_t          dstAddress_;
    string_t        dstIP_; 
    string_t        dstPort_;

    string_t        localIP_;
    string_t        versionStr_;
};

}//namespace http

#endif //HTTP_MESSAGE_H_
