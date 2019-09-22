#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "http/HttpHeaders.h"
#include "util/unp_time.h"
#include "http_parser/http_parser.h"
#include "util/string_piece/string_piece.h"
#include <boost/variant/variant.hpp>
#include <map>

namespace http{

class HttpMessage
{
public:
    using addr_t = net::inet_addr;
    using string_t = std::string;
    using CStringPiece_t = string_piece::const_string_piece;
public:
    HttpMessage();
    ~HttpMessage();

public:
    bool isDone() const {return isDone_;}
    template <typename T>
    T get() {return boost::get<T>(&message_);}


    void addHeader(string_t& field, CStringPiece_t value) { headers_.add(field, value); }
    void addHeader(string_t& field, string_t&& value) { headers_.add(field, value); }

private:
    boost::variant<boost::blank, HttpRequest, HttpResponse> message_;

    HttpHeaders     headers_;
    util::Time      startTime_;

    addr_t          dstAddress_;
    string_t        dstIP_; 
    string_t        dstPort_;
    string_t        localIP_;
    string_t        versionStr_;

    std::pair<uint8_t, uint8_t> version_;
    std::map<string_t, string_t> queryParams_;
    std::map<CStringPiece_t, CStringPiece_t> cookies_;

    bool            isDone_ = false;

    bool            chunked_ = 1;
    bool            upgraded_ = 1;
    bool            keepAlive_ = 1;
    bool            secure_ = 0;

};

}//namespace http

#endif //HTTP_MESSAGE_H_
