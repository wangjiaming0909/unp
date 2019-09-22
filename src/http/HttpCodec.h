#pragma once

#include "util/string_piece/string_piece.h"

class HttpCodec
{
public:
    using CStringPiece_t = string_piece::const_string_piece;
    struct Callback
    {
        virtual int onMessageBegin() = 0;
        virtual int onURL(const char* buf, size_t len) = 0;
        virtual int onReason(const char* buf, size_t len) = 0;
        virtual int onHeaderField(const char* buf, size_t len) = 0;
        virtual int onHeaderValue(const char* buf, size_t len) = 0;
        virtual int onHeadersComplete(size_t len) = 0;
        virtual int onBody(const char* buf, size_t len) = 0;
        virtual int onChunkHeader(size_t len) = 0;
        virtual int onChunkComplete() = 0;
        virtual int onMessageComplete() = 0;
    };

public:
    HttpCodec(){}
    virtual ~HttpCodec(){}

    virtual void setCallback(Callback* callback) = 0;
    virtual size_t onIngress(CStringPiece_t buf) = 0;
};