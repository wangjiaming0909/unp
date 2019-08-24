#include "HttpClientHandler.h"
#include "reactor/echo_connection_handler.h"
#include "http/http_parser/URLParser.h"

namespace examples
{


HttpClientHandler::HttpClientHandler(reactor::Reactor &react, const char* url, const char* userAgent) 
    : connection_handler(react)
    , url_{url}
    , userAgent_(userAgent)
    , req_{beast::http::verb::get, url_.begin(), 11}
{
    req_.set(beast::http::field::user_agent, userAgent_.begin());
    req_.set(beast::http::field::accept, "text/html");
    http::URLParser parser{url_};
    if(!parser.valid()) 
    {
        LOG(WARNING) << "url err: " << url;
        return;
    }
    req_.set(beast::http::field::host, parser.host().cbegin());
    req_.set(beast::http::field::connection, "keep-alive");
    // req_.set(http::field::keep_alive, "true");
}

HttpClientHandler::~HttpClientHandler()
{
}

int HttpClientHandler::open()
{
    beast::http::serializer<true, beast::http::string_body> seri{req_};
    reactor::WriteLambda writer{*this};
    beast::error_code err{};
    seri.next(err, writer);
    if(err && err != beast::errc::not_connected)
    {
        LOG(ERROR) << err;
        return -1;
    }
    enable_reading();
    return 0;
}

int HttpClientHandler::handle_input(int handle)
{
    connection_handler::handle_input(handle);
    beast::http::response_parser<beast::http::string_body> parser{};

    char* data = static_cast<char*>(::calloc(4096, 1));
    while(true)
    {
        if(input_buffer_.buffer_length() <= 0)
        {
            break;
        }
        auto beg = input_buffer_.begin().chain().get_buffer();
        memcpy(data, beg, input_buffer_.buffer_length());
        LOG(INFO) << data;
        ::memset(data, 0, 4096);
    }
}

}