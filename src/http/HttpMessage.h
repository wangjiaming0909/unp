#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "http/HttpHeaders.h"
#include "util/unp_time.h"
#include "http_parser/http_parser.h"
#include "util/string_piece/string_piece.h"
#include <boost/variant/variant.hpp>

namespace http{

class HttpMessage
{
public:
    using addr_t = net::inet_addr;
    using string_t = util::string;
public:
    HttpMessage(bool isRequest);
    ~HttpMessage();

public:
    bool isDone() const {return isDone_;}

private:
    boost::variant<boost::blank, HttpRequest, HttpResponse> message_;

    HttpHeaders     headers_;
    util::Time      startTime_;

    addr_t          dstAddress_;
    string_t        dstIP_; 
    string_t        dstPort_;
    string_t        localIP_;
    string_t        versionStr_;

    bool            isDone_ = false;

};

}//namespace http

#endif //HTTP_MESSAGE_H_
