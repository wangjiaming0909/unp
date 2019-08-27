#pragma once
#include "http/HttpMethod.h"
#include "http/HttpMessage.h"
#include "gperf/HttpHeaderCode.h"
#include "reactor/buffer.h"
#include <memory>

namespace http
{


template <typename T>
class HttpMessageBuilder
{
public:
    HttpMessageBuilder& method(http::HTTPMethod method)
    {
        boost::get<T>(&message_)->
    }
    HttpMessageBuilder& url(std::string& url)
    {
        message_->url_ = url;
    }
    HttpMessageBuilder& httpVersion(HttpVersion version)
    {
        message_->httpVersion_ = version;
    }
    HttpMessageBuilder& header(HttpHeaderCode headerCode, std::string& value)
    {
        message_->
    }
    HttpMessageBuilder& body(reactor::buffer& body);
    HttpMessageBuilder& eof();
    std::unique_ptr<HttpRequest> request();
private:
    reactor::buffer messageBuffer_;
    std::unique_ptr<HttpMessage> message_;
};

};
}