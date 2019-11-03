#pragma once
#include "reactor/connection_handler.h"
#include "http/Http1xCodec.h"
#include "http/http_parser/URLParser.h"
#include "http/Http1xCodec.h"

namespace examples
{

static string_piece::const_string_piece USERAGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:71.0) Gecko/20100101 Firefox/71.0";
static string_piece::const_string_piece ACCEPT = "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
static string_piece::const_string_piece ACCEPTENCODING = "gzip, deflate, br";

class HttpsClientHandler : public reactor::connection_handler, public http::HttpCodec::Callback{
public:
    HttpsClientHandler(reactor::Reactor &react, const char *url);

    void setUrl(const char *url) { url_ = url; }
    const std::string &getUrl() const { return url_; }

    int handle_input(int handle) override;

    int open() override;
    //HttpCodec::Callback methods
    int onStatus(const char* buf, size_t len) override;
    int onBody(const char* buf, size_t size) override;


private:
    int parseUrl(const std::string &url)
    {
        urlParser_.init(url);
        return urlParser_.valid();
    }

private:
    http::HttpMessage request_;
    http::URLParser urlParser_;
    http::Http1xCodec codec_;
    std::string url_;
};
}