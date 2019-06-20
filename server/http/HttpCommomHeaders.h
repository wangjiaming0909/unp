#ifndef _HTTP_COMMOM_HEADER_H_
#define _HTTP_COMMOM_HEADER_H_
#include "server/http/gperf/HttpCommomHeadersHash.h"

#include <vector>
#include <string>

namespace http
{
class HttpCommomHeaders
{
public:
    //if headerName is a valid headerName, return the header code, or return HTTP_HEADER_OTHER,
    static HttpHeaderCode getHeaderCode(const char* headerName, size_t len);

    static HttpHeaderCode getHeaderCode(const std::string& headerName);

    static const char* getPointerWithHeaderCode(HttpHeaderCode code);

public:
    static const size_t MAX_HEADER_LENGTH = MAX_WORD_LENGTH;
    static const size_t MIN_HEADER_LENGTH = MIN_WORD_LENGTH;

private:
    static const std::vector<std::string*>& getHeaderTable();
    static const std::vector<std::string*>& initHeaderTable();
};

}
#endif // _HTTP_COMMOM_HEADER_H_