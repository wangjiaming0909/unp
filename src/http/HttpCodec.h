#pragma once

#include "util/string_piece/string_piece.h"

namespace http
{

class HttpCodec
{
public:
    using CStringPiece_t = string_piece::const_string_piece;
    struct Callback
    {
        virtual int onMessageBegin(){}
        virtual int onURL(const char* buf, size_t len){}
        virtual int onStatus(const char*buf, size_t len){}
        virtual int onReason(const char* buf, size_t len){}
        virtual int onHeaderField(const char* buf, size_t len){}
        virtual int onHeaderValue(const char* buf, size_t len){}
        virtual int onHeadersComplete(size_t len){}
        virtual int onBody(const char* buf, size_t len){}
        virtual int onChunkHeader(size_t len){}
        virtual int onChunkComplete(){}
        virtual int onMessageComplete(){}
    };

public:
    HttpCodec(){}
    virtual ~HttpCodec(){}

    virtual void setCallback(Callback* callback) = 0;
    virtual size_t onIngress(CStringPiece_t buf) = 0;
};

}