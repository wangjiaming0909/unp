#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "http/HttpHeaders.h"
#include "util/unp_time.h"
#include "http_parser/http_parser.h"
#include "util/string_piece/string_piece.h"
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
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
    T& get() {return boost::get<T>(message_);}

    void addHeader(string_t& field, CStringPiece_t value)
    {
        if(field == HttpCommomHeaders::getPointerWithHeaderCode(HttpHeaderCode::HTTP_HEADER_COOKIE))
        {
            parseCookie(value);
        }
        headers_.add(field, value);
    }

    void addHeader(HttpHeaderCode field, CStringPiece_t value)
    {
        if(field == HttpHeaderCode::HTTP_HEADER_COOKIE)
        {
            parseCookie(value);
        }
        headers_.add(field, value);
    }

    void addHeader(string_t& field, string_t&& value)
    {
        if(field == HttpCommomHeaders::getPointerWithHeaderCode(HttpHeaderCode::HTTP_HEADER_COOKIE))
        {
            parseCookie(value);
        }
        headers_.add(field, std::move(value));
    }
    void setHttpVersion(uint8_t maj, uint8_t min)
    {
        version_.first = maj, version_.second = min;
        versionStr_ = std::to_string(maj).append(".").append(std::to_string(min));
    }

    const string_t getVersionStr();

    //request
    void setRequestMethod(http::HTTPMethod method)
    {
        if(message_.which() == 2) return;
        request().method_ = method;
    }

    void setRequestPath(const char* path)
    {
        if(message_.which() == 2) return;
        request().path_ = path;
    }

    void setRequestPath(string_piece::const_string_piece path)
    {
      if (message_.which() == 2) return;
      request().path_.assign(path.cbegin(), path.size());
    }

    std::shared_ptr<std::string> buildRequestLine();

    std::shared_ptr<std::string> buildRequestMessage()
    {
        if(message_.which() == 2) return nullptr;
        std::shared_ptr<std::string> ret = std::make_shared<std::string>();
        auto reqLine = buildRequestLine();
        auto headersStr = headers_.dump();
        ret->append(*reqLine);
        ret->append(*headersStr);
        return ret;
    }

    //response
    void setResponseStatus(uint16_t status)
    {
        if(message_.which() == 1) return;
        response().status_ = status;
    }

    const std::string* getHeaderValue(HttpHeaderCode code) const
    {
        return headers_[code];
    }
    bool hasHeader(HttpHeaderCode code) const;

private:
    HttpRequest& request()
    {
        if(message_.which() == 0)
        {
            message_ = HttpRequest();
        }
        return get<HttpRequest>();
    }
    HttpResponse& response()
    {
        if(message_.which() == 0)
        {
            message_ = HttpResponse();
        }
        return get<HttpResponse>();
    }

    void parseCookie(CStringPiece_t cookie)
    {
        CStringPiece_t subStr = cookie.sub_string(0, cookie.size());
        CStringPiece_t key{};
        CStringPiece_t value{};
        for ( ; subStr.size() > 0;)
        {
            auto posEqual = subStr.find('=');
            if(int(posEqual) < int(subStr.size()))
            {
                key.reset(subStr.cbegin(), subStr.cbegin() + posEqual);
                subStr = subStr.sub_string(posEqual + 1, subStr.size() - posEqual - 1);
            }else break;
            CStringPiece_t col = "; ";
            auto posCol = subStr.find(col);
            if(int(posCol) < int(subStr.size()))
            {
                value.reset(subStr.cbegin(), subStr.cbegin() + posCol);
                subStr = subStr.sub_string(posCol + 2, subStr.size() - posCol - 2);
            }
            if(posCol < 0) //the last cookie value
            {
                value = subStr;
            }
            if (key.size() != 0 && value.size() != 0)
            {
                cookies_.insert({key, value});
            }
        }
    }

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
