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
        virtual int onMessageBegin(){ return 0;}
        virtual int onURL(const char*, size_t){return 0;}
        virtual int onStatus(const char*, size_t){return 0;}
        virtual int onReason(const char*, size_t){return 0;}
        virtual int onHeaderField(const char*, size_t){return 0;}
        virtual int onHeaderValue(const char*, size_t){return 0;}
        virtual int onHeadersComplete(size_t){return 0;}
        virtual int onBody(const char*, size_t){return 0;}
        virtual int onChunkHeader(size_t){return 0;}
        virtual int onChunkComplete(){return 0;}
        virtual int onMessageComplete(){return 0;}
    };

public:
    HttpCodec(){}
    virtual ~HttpCodec(){}

    virtual void setCallback(Callback* callback) = 0;
    virtual size_t onIngress(CStringPiece_t buf) = 0;
};

}
