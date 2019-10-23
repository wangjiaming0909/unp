#include "http/Http1xCodec.h"
#include "http_parser/HttpParser.h"
#include "util/easylogging++.h"

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
    if(parser_->error() != 0) return 0;
    auto bytesParsed = parser_->parse(buf.cbegin(), buf.size());
    if(parser_->error() != 0)
    {
        LOG(ERROR) << "Parser error" << http_errno_name(http_errno(parser_->error()));
        return -1;
    }
    return bytesParsed;
}

int Http1xCodec::onMessageBegin()
{
    state_ = CodecState::ON_MESSAGEBEGIN;
    message_.reset(new HttpMessage{});
    if(callback_) callback_->onMessageBegin();
    return 0;
}

int Http1xCodec::onStatus(const char* buf, size_t len)
{
    state_ = CodecState::ON_STATUS;

    if(callback_) callback_->onStatus(buf, len);
    return 0;
}

int Http1xCodec::onURL(const char* buf, size_t len)
{
    state_ = CodecState::ON_URL;
    url_.append(buf, len);
    if(callback_) callback_->onURL(buf, len);
    return 0;
}

int Http1xCodec::onReason(const char* buf, size_t len)
{
    if(callback_) callback_->onReason(buf, len);
    return 0;
}

int Http1xCodec::onHeaderField(const char* buf, size_t len)
{
    // assert(state_ == CodecState::)
    assert(currentHeaderFiled_.empty());
    currentHeaderFiled_.append(buf, len);
    state_ = CodecState::ON_HEADERFIELD;
    if(callback_) callback_->onHeaderField(buf, len);
    return 0;
}
int Http1xCodec::onHeaderValue(const char* buf, size_t len)
{
    currentHeaderValue_.reset(buf, len);
    message_->addHeader(currentHeaderFiled_, currentHeaderValue_);
    currentHeaderFiled_.clear();
    currentHeaderValue_.reset();
    state_ = CodecState::ON_HEADERVALUE;
    if(callback_) callback_->onHeaderValue(buf, len);
    return 0;
}
int Http1xCodec::onHeadersComplete(size_t len)
{
    state_ = CodecState::ON_HEADERS_COMPLETE;
    if(callback_) callback_->onHeadersComplete(len);
    return 0;
}
int Http1xCodec::onBody(const char* buf, size_t len)
{
    state_ = CodecState::ON_BODY;
    if(callback_) callback_->onBody(buf, len);
    return 0;
}
int Http1xCodec::onChunkHeader(size_t len)
{
    state_ = CodecState::ON_CHUNKHEADER;
    if(callback_) callback_->onChunkHeader(len);
    return 0;
}
int Http1xCodec::onChunkComplete()
{
    state_ = CodecState::ON_CHUNKCOMPLETE;
    if(callback_) callback_->onChunkComplete();
    return 0;
}
int Http1xCodec::onMessageComplete()
{
    state_ = CodecState::ON_MESSAGE_COMPLETE;
    if(callback_) callback_->onMessageComplete();
    return 0;
}

int Http1xCodec::onParserError()
{

}

uint64_t Http1xCodec::contentLength() const {return parser_->contentLength();}
short Http1xCodec::httpMajor() const {return parser_->httpMajor();}
short Http1xCodec::httpMinor() const {return parser_->httpMinor();}
int Http1xCodec::status() const {return parser_->statusCode();}
int Http1xCodec::method() const {return parser_->method();}
int Http1xCodec::upgrade() const {return parser_->upgrade();}
http_parser_type Http1xCodec::parserType() const {return parser_->parserType();}
}