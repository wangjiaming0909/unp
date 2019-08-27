#pragma once

#include "reactor/connection_handler.h"
#include "util/string_piece/string_piece.h"
#include "boost/beast/http.hpp"
#include "boost/beast/core.hpp"


namespace examples
{

using namespace boost;

class HttpClientHandler : public reactor::connection_handler
{
private:
    string_piece::const_string_piece url_;
    string_piece::const_string_piece userAgent_;
    beast::http::request<beast::http::string_body> req_;
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
}