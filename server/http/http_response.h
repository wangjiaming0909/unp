#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include "server/util/XString.h"

struct http_response
{
    using string_t = util::string;
    uint16_t    status_;
    string_t    statusStr_;
    string_t    statusMsg_;
};

#endif //HTTP_RESPONSE_H_