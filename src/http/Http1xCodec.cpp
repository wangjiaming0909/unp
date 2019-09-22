#include "http/Http1xCodec.h"
#include "http_parser/HttpParser.h"

namespace http
{


Http1xCodec::Http1xCodec(HttpDirection direction)
    : callback_{nullptr}
    , parser_{nullptr}
{
    switch (direction)
    {
    case HttpDirection::UPSTREAM:
        parser_ = std::make_unique<HttpParserWrapper>(*this, http_parser_type::HTTP_REQUEST);
        break;
    case HttpDirection::DOWNSTREAM:
        parser_ = std::make_unique<HttpParserWrapper>(*this, http_parser_type::HTTP_RESPONSE);
        break;
    default:
        parser_ = std::make_unique<HttpParserWrapper>(*this, http_parser_type::HTTP_BOTH);
        break;
    }
}

Http1xCodec::~Http1xCodec()
{
}

void Http1xCodec::setCallback(Callback* callback)
{

}

size_t Http1xCodec::onIngress(CStringPiece_t buf)
{
    if(parser_->getParserError() != 0) return 0;
    auto bytesParsed = parser_->parse(buf.cbegin(), buf.size());
    return bytesParsed;
}

int Http1xCodec::onMessageBegin()
{
    state_ = CodecState::ON_MESSAGEBEGIN;
    message_.reset(new HttpMessage{});
    callback_->onMessageBegin();
    return 0;
}
int Http1xCodec::onURL(const char* buf, size_t len)
{
    state_ = CodecState::ON_URL;
    url_.append(buf, len);
    callback_->onURL(buf, len);
    return 0;
}
int Http1xCodec::onReason(const char* buf, size_t len)
{
    callback_->onReason(buf, len);
    return 0;
}
int Http1xCodec::onHeaderField(const char* buf, size_t len)
{
    // assert(state_ == CodecState::)
    assert(currentHeaderFiled_.empty());
    currentHeaderFiled_.append(buf, len);
    state_ = CodecState::ON_HEADERFIELD;
    callback_->onHeaderField(buf, len);
    return 0;
}
int Http1xCodec::onHeaderValue(const char* buf, size_t len)
{
    currentHeaderValue_.reset(buf, len);
    message_->addHeader(currentHeaderFiled_, currentHeaderValue_);
    currentHeaderFiled_.clear();
    currentHeaderValue_.reset();
    state_ = CodecState::ON_HEADERVALUE;
    callback_->onHeaderValue(buf, len);
    return 0;
}
int Http1xCodec::onHeadersComplete(size_t len)
{
    state_ = CodecState::ON_HEADERS_COMPLETE;
    callback_->onHeadersComplete(len);
    return 0;
}
int Http1xCodec::onBody(const char* buf, size_t len)
{
    state_ = CodecState::ON_BODY;
    callback_->onBody(buf, len);
    return 0;
}
int Http1xCodec::onChunkHeader(size_t len)
{
    state_ = CodecState::ON_CHUNKHEADER;
    callback_->onChunkHeader(len);
    return 0;
}
int Http1xCodec::onChunkComplete()
{
    state_ = CodecState::ON_CHUNKCOMPLETE;
    callback_->onChunkComplete();
    return 0;
}
int Http1xCodec::onMessageComplete()
{
    state_ = CodecState::ON_MESSAGE_COMPLETE;
    callback_->onMessageComplete();
    return 0;
}
}