#pragma once

#include "reactor/sock_connection_handler.h"
#include "util/string_piece/string_piece.h"
#include "boost/beast/http.hpp"
#include "boost/beast/core.hpp"
#include "util/XString.h"
#include "util/FileWriter.h"
#include "http/HttpCodec.h"
#include "http/Http1xCodec.h"
#include <memory>


namespace examples
{

using namespace boost;

class HttpClientHandler : public reactor::sock_connection_handler
{
private:
    string_piece::const_string_piece url_;
    string_piece::const_string_piece userAgent_;
    beast::http::request<beast::http::empty_body> req_;
    beast::http::response<beast::http::string_body> res_;
public:
    HttpClientHandler(reactor::Reactor &react, const char* url, const char* userAgent, const std::string& displayName);
    virtual ~HttpClientHandler();

    virtual int open();
    virtual int handle_input(int handle);
private:
    int get();
    std::string name_;
};


class HttpDownloader : public reactor::sock_connection_handler, public http::HttpCodec::Callback
{
public:
    HttpDownloader(reactor::Reactor &react, const char* url, const char* userAgent, const std::string& displayName);
    virtual ~HttpDownloader();

    virtual int open();
    virtual int handle_input(int handle);
    int get();

    int onStatus(const char* buf, size_t len) override;
    virtual int onChunkBody(std::uint64_t remain,   // The number of bytes left in this chunk
            string_view body,       // A buffer holding chunk body data
            beast::error_code& ec);         // We can set this to indicate an error

    int onBody(const char* buf, size_t size) override;

private:
    void init();
private:
    std::string url_;
    std::string userAgent_;
    beast::http::request<beast::http::empty_body> request_;
    beast::http::response<beast::http::buffer_body> response_;
    reactor::buffer inputData_;
    bool isCurrentChunkDone = false;
    std::string name_;
    static const int DEFAULTBODYSIZE = 4096;
    char bodyData_[DEFAULTBODYSIZE];
    utils::FileWriter writer_;
    std::shared_ptr<beast::http::response_parser<beast::http::buffer_body>> responseParser_;
    std::shared_ptr<http::Http1xCodec> codec_;
};
}
