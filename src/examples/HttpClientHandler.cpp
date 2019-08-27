#include "HttpClientHandler.h"
#include "reactor/echo_connection_handler.h"
#include "http/http_parser/URLParser.h"
#include "config/json11.hpp"
#include <thread>
#include <iostream>
#include "util/unp_time.h"
#include <chrono>
#include <iomanip>

namespace examples
{


HttpClientHandler::HttpClientHandler(reactor::Reactor &react, const char* url, const char* userAgent, const std::string& displayName) 
    : connection_handler(react)
    , url_{url}
    , userAgent_(userAgent)
    , req_{beast::http::verb::get, url_.begin(), 11}
    , name_(displayName)
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
    if(get() != 0) return -1;
    enable_reading();
    return 0;
}

int HttpClientHandler::get()
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
    return 0;
}

int HttpClientHandler::handle_input(int handle)
{
    bool shouldCloseHandle = false;
    if(connection_handler::handle_input(handle) < 0)
    {
        shouldCloseHandle = true;
    }
    beast::http::response_parser<beast::http::string_body> parser{};

    std::string chunk;
    auto body_cb =
        [&](std::uint64_t remain,   // The number of bytes left in this chunk
            string_view body,       // A buffer holding chunk body data
            beast::error_code& ec)         // We can set this to indicate an error
        {
            // If this is the last piece of the chunk body,
            // set the error so that the call to `read` returns
            // and we can process the chunk.
            if(remain == body.size())
                ec = beast::http::error::end_of_chunk;

            // Append this piece to our container
            chunk.append(body.data(), body.size());

            // The return value informs the parser of how much of the body we
            // consumed. We will indicate that we consumed everything passed in.
            return body.size();
        };

    parser.on_chunk_body(body_cb);

    char* data = static_cast<char*>(::calloc(4096, 1));
    if(input_buffer_.buffer_length() > 0)
    {
        auto beg = input_buffer_.begin().chain().get_buffer();
        LOG(INFO) << static_cast<const char*>(beg);
        input_buffer_.pullup(input_buffer_.buffer_length());
        void* data = ::calloc(4096, 1);
        input_buffer_.copy_out_from(data, input_buffer_.buffer_length(), input_buffer_.begin());
        // boost::asio::const_buffer buf{beg, input_buffer_.buffer_length()};
        boost::asio::const_buffer buf{data, input_buffer_.buffer_length()};

        beast::error_code err;
        parser.eager(true);
        parser.put(buf, err);
        parser.put_eof(err); 
        LOG(WARNING) << err.message();
        free(data);
    }
    if(chunk.length() != 0)
    {
        std::string err;
        auto parseredJson = json11::Json::parse(chunk, err);
        auto map = parseredJson.object_items();
        if(map.count("data") == 0)
        {
            LOG(WARNING) << "json error no data tag";
            return -1;
        }
        map = map["data"].object_items();
        std::string key = "1A0001";
        if(map.count(key) == 0) 
        {
            LOG(WARNING) << "json error no " << key;
            return -1;
        }

        std::string SHANGZHENGZHISHU_KEY = "199112";
        auto SHANGZHENGZHISHU_MAP = map[key].object_items();
        if(SHANGZHENGZHISHU_MAP.count(SHANGZHENGZHISHU_KEY) == 0)
        {
            LOG(WARNING) << "json error no " << SHANGZHENGZHISHU_KEY;
            return -1;
        }

        // LOG(INFO) << "-----------------------------------上证指数: " << SHANGZHENGZHISHU_MAP[SHANGZHENGZHISHU_KEY].string_value() << "---------------------------";

        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::cout << std::put_time(std::localtime(&now_time_t), "%F %T");
        std::cout << name_ << ": " << SHANGZHENGZHISHU_MAP[SHANGZHENGZHISHU_KEY].string_value() << "---------------------------" << std::endl;;
        // std::this_thread::sleep_for(2s);
        input_buffer_.drain(input_buffer_.buffer_length());
        // get();
    }
    else
    {
        LOG(INFO) << "Not completed...";
    }
    if(shouldCloseHandle) return -1;
}

}