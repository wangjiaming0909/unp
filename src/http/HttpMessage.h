#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "http/HttpHeaders.h"
#include "util/unp_time.h"
#include <boost/variant/variant.hpp>

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
    boost::variant<boost::blank, HttpRequest, HttpResponse> message_;

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
