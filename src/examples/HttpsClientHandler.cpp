#include "examples/HttpsClientHandler.h"

namespace examples
{

HttpsClientHandler::HttpsClientHandler(reactor::Reactor& react, const char* url, MessageSetupCallback_t&& callback) 
    : sock_connection_handler{react, true}
    , request_{}
    , urlParser_{}
    , codec_{http::HttpDirection::DOWNSTREAM}
    , url_{url}
    , mesSetupCallback_{callback}
    , writer_{"/tmp/unp.zip"}
{
    codec_.setCallback(this);
    if (!parseUrl(url_))
    {
        url = "";
        return;
    }
}

int HttpsClientHandler::open()
{
    // request_.setRequestMethod(http::HTTPMethod::GET);
    // request_.setHttpVersion(1, 1);
    request_.setRequestPath(urlParser_.path().cbegin());
    request_.addHeader(http::HttpHeaderCode::HTTP_HEADER_HOST, urlParser_.host());
    // request_.addHeader(http::HttpHeaderCode::HTTP_HEADER_USER_AGENT, USERAGENT);
    // request_.addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT, ACCEPT);
    // request_.addHeader(http::HttpHeaderCode::HTTP_HEADER_ACCEPT_ENCODING, ACCEPTENCODING);
    int ret = 0;
    if(mesSetupCallback_)
    {
        ret = mesSetupCallback_(request_);
    }
    if(ret != 0) return -1;

    auto messageStr = request_.buildRequestMessage();
    LOG(INFO) << *messageStr;
    auto bytesWritten = write(messageStr->c_str(), messageStr->size());
    if (bytesWritten <= 0) return -1;

    return enable_reading();
}

int HttpsClientHandler::handle_input(int handle)
{
    if(sock_connection_handler::handle_input(handle) < 0)
    {
        LOG(WARNING) << "error when sock_connection_handler::handle_input";
        return -1;
    }

    if(input_buffer_.buffer_length() == 0) 
    {
        LOG(INFO) << "didn't get any data...";
        return 0;
    }

    while(input_buffer_.buffer_length() > 0)
    {
        auto firstChain = input_buffer_.begin().chain();
        auto data = firstChain.get_start_buffer();
        auto chainLen = firstChain.size();
        string_piece::const_string_piece sp{static_cast<const char*>(data), chainLen};
        size_t bytesRead = codec_.onIngress(sp);
        if(codec_.hasError())
        {
            LOG(WARNING) << "Http parser parse error";
            codec_.pause(1);
            return -1;
        }
        assert(bytesRead == chainLen);
        input_buffer_.drain(chainLen);
    }
    return 0;

}

int HttpsClientHandler::onStatus(const char*, size_t)
{
    if(codec_.status() != 200)
    {
        LOG(WARNING) << "status is: " << codec_.status();
        return -1;
    }
    return 0;
}

int HttpsClientHandler::onBody(const char* buf, size_t size)
{
    writer_.write(buf, size);
    writer_.flush();
    return 0;
}

}
