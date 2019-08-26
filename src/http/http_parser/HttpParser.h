#ifndef _HTTP_PARSER_H_
#define _HTTP_PARSER_H_

#include "http_parser.h"

namespace http
{

class HttpPaser
{
public:
    HttpPaser(http_parser_type parserType);
    ~HttpPaser();

public:
    void setHttpParserSettings(http_parser_settings& settings) {parserSettings_ = settings;}

    int put();

public:
    void setOnUrlCB(http_data_cb cb) {parserSettings_.on_url = cb;}
    void setOnHeaderFieldCB(http_data_cb cb) {parserSettings_.on_header_field = cb;}
    void setOnHeaderValueCB(http_data_cb cb) {parserSettings_.on_header_value = cb;}
    void setOnHeadersCompleteCB(http_cb cb) {parserSettings_.on_headers_complete = cb;}
    void setOnBodyCB(http_data_cb cb) {parserSettings_.on_body = cb;}
    void setOnMessageBeginCB(http_cb cb) {parserSettings_.on_message_begin = cb;}
    void setOnMessageCompleteCB(http_cb cb) {parserSettings_.on_message_complete = cb;}
    // void setOnReasonCB(http_data_cb cb) {parserSettings_.}
    void setOnChunkHeaderCB(http_cb cb) {parserSettings_.on_chunk_header = cb;}
    void setOnChunkCompleteCB(http_cb cb) {parserSettings_.on_chunk_complete = cb;}

private:
    http_parser_settings        parserSettings_;
    http_parser                 parser_;
};


}

#endif //_HTTP_PARSER_H_