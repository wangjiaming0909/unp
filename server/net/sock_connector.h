#ifndef _UNP_SOCK_CONNECTOR_H_
#define _UNP_SOCK_CONNECTOR_H_

#include "inet_sock.h"
#include <chrono>

namespace net{
class sock_connector{
public:
    sock_connector(){}
    ~sock_connector(){}
    // ssize_t recv(
    //     void* buf, 
    //     size_t len, ) const;
    

private:
    inet_sock* sock_fd;
};


}

#endif // _UNP_SOCK_CONNECTOR_H_