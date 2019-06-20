#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_
#include <string>

namespace http
{
struct HttpResponse
{
    using string_t = std::string;
    uint16_t    status_;
    string_t    statusStr_;
    string_t    statusMsg_;
};

}//http
#endif //HTTP_RESPONSE_H_