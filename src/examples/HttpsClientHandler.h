#pragma once
#include "reactor/sock_connection_handler.h"
#include "http/Http1xCodec.h"
#include "http/http_parser/URLParser.h"
#include "http/Http1xCodec.h"
#include "util/FileWriter.h"

namespace examples
{


class HttpsClientHandler : public reactor::sock_connection_handler, public http::HttpCodec::Callback{
public:
    using MessageSetupCallback_t = std::function<int(http::HttpMessage& mes)>;
    HttpsClientHandler(reactor::Reactor &react, const char *url, MessageSetupCallback_t&& callback);
    ~HttpsClientHandler()
    {
        if(writer_.isValid()) writer_.close();
    }

    void setUrl(const char *url) { url_ = url; }
    const std::string &getUrl() const { return url_; }

    virtual int handle_input(int handle) override;

    virtual int open() override;
    //HttpCodec::Callback methods
    virtual int onStatus(const char* buf, size_t len) override;
    virtual int onBody(const char* buf, size_t size) override;


protected:
    bool parseUrl(const std::string &url)
    {
        urlParser_.init(url);
        return urlParser_.valid();
    }

private:
    http::HttpMessage request_;
    http::URLParser urlParser_;
    http::Http1xCodec codec_;
    std::string url_;
    MessageSetupCallback_t mesSetupCallback_;
    utils::FileWriter writer_;
};
}