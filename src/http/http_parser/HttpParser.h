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
    unsigned int getParserError() const {return parser_.http_errno;}
    size_t parse(const char* buf, size_t len);
private:
    http_parser_settings        parserSettings_;
    http_parser                 parser_;

    bool parserError_ = false;


    static int onMessageBeginCB(http_parser* parser);
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