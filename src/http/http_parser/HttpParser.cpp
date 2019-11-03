#include "HttpParser.h"
#include "http/Http1xCodec.h"
#include "util/easylogging++.h"

namespace http
{


HttpParserWrapper::HttpParserWrapper(Http1xCodec& codec, http_parser_type parserType)
    : parserSettings_{}
    , parser_{}
{
    http_parser_init(&parser_, parserType);
    parser_.data = &codec;
}

HttpParserWrapper::~HttpParserWrapper()
{
}


const http_parser_settings* HttpParserWrapper::getParserSettings()
{
    static http_parser_settings settings = []
    {
        http_parser_settings st;
        st.on_message_begin = HttpParserWrapper::onMessageBeginCB;
        st.on_url = HttpParserWrapper::onUrlCB;
        st.on_status = HttpParserWrapper::onStatusCB;
        st.on_header_field = HttpParserWrapper::onHeaderFieldCB;
        st.on_header_value = HttpParserWrapper::onHeaderValueCB;
        st.on_headers_complete = HttpParserWrapper::onHeadersCompleteCB;
        st.on_body = HttpParserWrapper::onBodyCB;
        st.on_message_complete = HttpParserWrapper::onMessageCompleteCB;
        // st.on_reason = HttpParserWrapper::onReasonCB;
        st.on_chunk_header = HttpParserWrapper::onChunkHeaderCB;
        st.on_chunk_complete = HttpParserWrapper::onChunkCompleteCB;
        return st;
    }();
    return &settings;
}

int HttpParserWrapper::onMessageBeginCB(http_parser* parser)
{
    auto* codec = static_cast<Http1xCodec*>(parser->data);
    assert(codec != nullptr);
    codec->onMessageBegin();
    return 0;
}
int HttpParserWrapper::onPathCB(http_parser* parser, const char* buf, size_t len)
{
    return 0;
}
int HttpParserWrapper::onQueryStringCB(http_parser* parser, const char* buf, size_t len)
{
    return 0;
}
int HttpParserWrapper::onUrlCB(http_parser* parser, const char* buf, size_t len)
{
    auto* codec = static_cast<Http1xCodec*>(parser->data);
    assert(codec != nullptr);
    codec->onURL(buf, len);
    return 0;
}

int HttpParserWrapper::onReasonCB(http_parser* parser, const char* buf, size_t len)
{
    return 0;
}
int HttpParserWrapper::onHeaderFieldCB(http_parser* parser, const char* buf, size_t len)
{
    auto* codec = static_cast<Http1xCodec*>(parser->data);
    assert(codec != nullptr);
    codec->onHeaderField(buf, len);
    return 0;
}
int HttpParserWrapper::onHeaderValueCB(http_parser* parser, const char* buf, size_t len)
{
    auto* codec = static_cast<Http1xCodec*>(parser->data);
    assert(codec != nullptr);
    codec->onHeaderValue(buf, len);
    return 0;
}
int HttpParserWrapper::onHeadersCompleteCB(http_parser* parser)
{
    auto* codec = static_cast<Http1xCodec*>(parser->data);
    assert(codec != nullptr);
    codec->onHeadersComplete(0);
    return 0;
}
int HttpParserWrapper::onBodyCB(http_parser* parser, const char* buf, size_t len)
{
    auto* codec = static_cast<Http1xCodec*>(parser->data);
    assert(codec != nullptr);
    codec->onBody(buf, len);
    return 0;
}
int HttpParserWrapper::onChunkHeaderCB(http_parser* parser)
{
    return 0;
}
int HttpParserWrapper::onChunkCompleteCB(http_parser* parser)
{
    return 0;
}
int HttpParserWrapper::onMessageCompleteCB(http_parser* parser)
{
    auto* codec = static_cast<Http1xCodec*>(parser->data);
    assert(codec != nullptr);
    codec->onMessageComplete();
    return 0;
}

int HttpParserWrapper::onStatusCB(http_parser* parser, const char* buf, size_t len)
{
    auto* codec = static_cast<Http1xCodec*>(parser->data);
    assert(codec != nullptr);
    codec->onStatus(buf, len);
    return 0;
}

int HttpParserWrapper::parserPause(int pause)
{
    auto err = parser_.http_errno;
    if(err != HPE_OK && err != HPE_PAUSED)
    {
        LOG(WARNING) << "pause a parser error";
        return -1;
    }
    http_parser_pause(&parser_, pause);
    return 0;
}
}
