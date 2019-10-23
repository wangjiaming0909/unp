#pragma once

#include "http/HttpCodec.h"
#include "http/HttpMessage.h"
#include "http/HttpDirection.h"
#include "util/string_piece/string_piece.h"
#include "net/unp.h"

namespace http
{

class HttpParserWrapper;

class Http1xCodec : public HttpCodec
{
public:
    using String_t = std::string;
    friend class HttpParserWrapper;

TEST_PRIVATE:
    enum class CodecState : uint8_t
    {
        ERROR,
        IDLE,
        ON_URL,
        ON_STATUS,
        ON_MESSAGEBEGIN,
        ON_HEADERFIELD,
        ON_HEADERVALUE,
        ON_HEADERS_COMPLETE,
        ON_CHUNKHEADER,
        ON_CHUNKCOMPLETE,
        ON_BODY,
        ON_MESSAGE_COMPLETE
    };

public:
    Http1xCodec(HttpDirection direction);
    ~Http1xCodec();

public:
    virtual void setCallback(Callback* callback) override;
    virtual size_t onIngress(CStringPiece_t buf) override;

    uint64_t contentLength() const;
    short httpMajor() const;
    short httpMinor() const;
    int status() const;
    int method() const;
    int upgrade() const;
    http_parser_type parserType() const;
    bool isPaused() const;


TEST_PRIVATE:
  int onMessageBegin();
  int onURL(const char* buf, size_t len);
  int onStatus(const char* buf, size_t len);
  int onReason(const char* buf, size_t len);
  int onHeaderField(const char* buf, size_t len);
  int onHeaderValue(const char* buf, size_t len);
  int onHeadersComplete(size_t len);
  int onBody(const char* buf, size_t len);
  int onChunkHeader(size_t len);
  int onChunkComplete();
  int onMessageComplete();
  int onParserError();

TEST_PRIVATE:
    std::unique_ptr<HttpMessage> message_;
    Callback*               callback_;
    std::unique_ptr<HttpParserWrapper> parser_;


    String_t                url_;
    String_t                currentHeaderFiled_;
    CStringPiece_t          currentHeaderValue_;
    CodecState              state_ = CodecState::IDLE;
};

}
