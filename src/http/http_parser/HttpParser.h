#ifndef _HTTP_PARSER_H_
#define _HTTP_PARSER_H_

#include "http_parser.h"
#include "http/HttpMessage.h"


namespace http
{
class Http1xCodec;

class HttpParserWrapper
{
public:
    HttpParserWrapper(Http1xCodec& codec, http_parser_type parserType);
    ~HttpParserWrapper();

public:
    inline unsigned int error() const {return parser_.http_errno;}
    inline size_t parse(const char* buf, size_t len)
    {
        return http_parser_execute(&parser_, getParserSettings(), buf, len);
    }
    int parserPause(int pause);

    inline int state(){return parser_.state;}
    inline uint64_t contentLength() const {return parser_.content_length;}
    inline short httpMajor() const {return parser_.http_major;}
    inline short httpMinor() const {return parser_.http_minor;}
    inline int statusCode() const {return parser_.status_code;}
    inline int method() const {return parser_.method;}
    inline int upgrade() const {return parser_.upgrade;}
    inline http_parser_type parserType() const {return static_cast<http_parser_type>(parser_.type);}
    inline bool isPaused() const {return error() == HPE_PAUSED;}

private:
    http_parser_settings        parserSettings_;
    http_parser                 parser_;

    bool parserError_ = false;


    static int onMessageBeginCB(http_parser* parser);
    static int onStatusCB(http_parser* parser, const char* buf, size_t len);
    static int onPathCB(http_parser* parser, const char* buf, size_t len);
    static int onQueryStringCB(http_parser* parser, const char* buf, size_t len);
    static int onUrlCB(http_parser* parser, const char* buf, size_t len);
    static int onReasonCB(http_parser* parser, const char* buf, size_t len);
    static int onHeaderFieldCB(http_parser* parser, const char* buf, size_t len);
    static int onHeaderValueCB(http_parser* parser, const char* buf, size_t len);
    static int onHeadersCompleteCB(http_parser* parser);
    static int onBodyCB(http_parser* parser, const char* buf, size_t len);
    static int onChunkHeaderCB(http_parser* parser);
    static int onChunkCompleteCB(http_parser* parser);
    static int onMessageCompleteCB(http_parser* parser);

    static const http_parser_settings* getParserSettings();
};
}


#endif //_HTTP_PARSER_H_