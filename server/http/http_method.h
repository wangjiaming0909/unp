#ifndef _HTTP_METHOD_H_
#define _HTTP_METHOD_H_

namespace http
{

enum class http_method
{
    GET,
    POST,
    OPTIONS,
    DELETE,
    HEAD,
    CONNECT,
    PUT,
    TRACE,
    PATCH,
    SUB,
    PUB,
    UNSUB
};
} // namespace http
#endif //_HTTP_METHOD_H_