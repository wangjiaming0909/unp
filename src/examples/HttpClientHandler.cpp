#include "HttpClientHandler.h"
#include "reactor/echo_connection_handler.h"
#include "http/http_parser/URLParser.h"
#include "config/json11.hpp"
#include <thread>
#include <iostream>
#include "util/unp_time.h"
#include <chrono>
#include <iomanip>
#include "boost/beast/http/buffer_body.hpp"

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
    beast::http::serializer<true, beast::http::empty_body> seri{req_};
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

    if(input_buffer_.buffer_length() > 0)
    {
        // auto beg = input_buffer_.begin().chain().get_buffer();
        // LOG(INFO) << static_cast<const char*>(beg);
        auto data = input_buffer_.pullup(input_buffer_.buffer_length());
        // void* data = ::calloc(4096, 1);
        // input_buffer_.copy_out_from(data, input_buffer_.buffer_length(), input_buffer_.begin());
        // boost::asio::const_buffer buf{beg, input_buffer_.buffer_length()};
        boost::asio::const_buffer buf{data, input_buffer_.buffer_length()};

        beast::error_code err;
        parser.eager(true);
        parser.put(buf, err);
        parser.put_eof(err); 
        LOG(WARNING) << err.message();
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
    return 0;
}

HttpDownloader::HttpDownloader(reactor::Reactor &react, const char* url, const char* userAgent, const std::string& displayName)
    : connection_handler(react)
    , url_{url}
    , userAgent_(userAgent)
    , request_{beast::http::verb::get, url_.c_str(), 11}
    , response_{}
    , inputData_{}
    , name_(displayName)
    , bodyData_()
    , writer_{"/tmp/unp.tmp"}
    , responseParser_{nullptr}
{
    init(url, userAgent);
}

void HttpDownloader::init(const char* url, const char* userAgent)
{
    request_.set(beast::http::field::user_agent, userAgent_.c_str());
    request_.set(beast::http::field::accept, "text/html");
    http::URLParser parser{url_};
    if(!parser.valid()) 
    {
        LOG(WARNING) << "url err: " << url;
        return;
    }
    auto hostSize = parser.host().size();
    char host[hostSize];
    memset(host, 0, hostSize);
    memcpy(host, parser.host().cbegin(), hostSize);
    request_.set(beast::http::field::host, &*host);
    request_.set(beast::http::field::connection, "keep-alive");

    auto& body = response_.body();
    body.data = &*bodyData_;
    body.size = DEFAULTBODYSIZE;

    responseParser_.reset(new beast::http::response_parser<beast::http::buffer_body>(response_));
    responseParser_->body_limit(UINT64_MAX);
    auto onChunkBody = std::bind(&HttpDownloader::onChunkBody, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    responseParser_->on_chunk_body(onChunkBody);
}

HttpDownloader::~HttpDownloader()
{
    if(writer_.isValid()) writer_.close();
}

int HttpDownloader::open()
{
    if(get() != 0) return -1;
    return enable_reading();
}

int HttpDownloader::get()
{
    beast::http::serializer<true, beast::http::empty_body> seri{request_};
    reactor::WriteLambda writer{*this};
    beast::error_code err{};
    seri.next(err, writer);
    if(err && err != beast::errc::not_connected)
    {
        LOG(ERROR) << err.message();
        return -1;
    }
    return 0;
}

int HttpDownloader::handle_input(int handle)
{
    int ret = connection_handler::handle_input(handle);
    if(ret < 0)
    {
        LOG(WARNING) << "error when connection_handler::handle_input";
        return -1;
    }

    if(input_buffer_.buffer_length() == 0) 
    {
        LOG(INFO) << "didn't get any data...";
        return 0;
    }
    boost::beast::error_code errCode;

    while(input_buffer_.buffer_length() > 0)
    {
        auto data = input_buffer_.pullup(input_buffer_.buffer_length());
        boost::asio::const_buffer bufToParser{data, input_buffer_.buffer_length()};
        auto bytesConsumed = responseParser_->put(bufToParser, errCode);
        if(errCode && errCode.failed())
        {
            LOG(WARNING) << "parser error: " << errCode.message();
            return -1;
        }
        if(responseParser_->is_header_done())
        {
            if(responseParser_->get().result_int() != 200)
            {
                return -1;
            }
        }

        if(!responseParser_->is_done())
        {
            LOG(INFO) << "Not done...";
        }
        if(responseParser_->chunked())
        {

        }
        else if( responseParser_->is_header_done() && responseParser_->content_length_remaining().get() > 0)
        {
            auto remain = responseParser_->content_length_remaining().get();
            auto length = responseParser_->content_length().get();
            if(remain != length && remain > 0) 
            {
                writer_.write(bodyData_, DEFAULTBODYSIZE - response_.body().size);
                response_.body().size = DEFAULTBODYSIZE;
            }
        }
        if(responseParser_->is_done()) 
        {
            writer_.write(bodyData_, DEFAULTBODYSIZE - response_.body().size);
            response_.body().size = DEFAULTBODYSIZE;
        }
        input_buffer_.drain(bytesConsumed);
    }
    return 0;
}

int HttpDownloader::onChunkBody( std::uint64_t remain, string_view body, beast::error_code& ec)
{
    auto sizeInCurrentChunk = body.size();
    if(remain - sizeInCurrentChunk == 0) isCurrentChunkDone = true;
    if(sizeInCurrentChunk <= 0) return 0;
    inputData_.append(body.data(), sizeInCurrentChunk);
    return sizeInCurrentChunk;
}

}