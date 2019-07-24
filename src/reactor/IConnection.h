#ifndef REACTOR_ICONNECTION__H_
#define REACTOR_ICONNECTION__H_

#include "net/sock_stream.h"
namespace reactor
{
struct IConnection
{
    virtual net::sock_stream& get_sock_stream() = 0;
    virtual int open() = 0;
    virtual int close() = 0;
};

}
#endif //REACTOR_ICONNECTION__H_