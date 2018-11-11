#ifndef _UNP_SOCK_STREAM_H_
#define _UNP_SOCK_STREAM_H_
#include <chrono>
#include <sys/socket.h>
#include "inet_sock.h"
#include <unistd.h>
namespace net{

class sock_stream{
public:
    using micro_seconds = std::chrono::microseconds; 
    ssize_t recv( void* buffer, size_t len, 
        const micro_seconds* timeout = 0) const{
        return recv_imp(buffer, len, timeout);
    }
    ssize_t recvv(
        iovec iov[], int n,
        const micro_seconds* timeout = 0)const{

    }

    ssize_t send( const void* buffer, size_t len, int flags,
        const micro_seconds* timeout = 0)const{
        return ::send(this->sock_fd_->get_handler(),
                      buffer, len, flags);
    }
    ssize_t sendv(
        const iovec iov[],
        int n,
        const micro_seconds* timeout = 0) const;

private:
    ssize_t recv_imp(void* buffer, size_t len,
        const micro_seconds* timeout = 0) const{
        if(timeout == 0){
            return ::read(sock_fd_->get_handler(), buffer, len);
        } else {
            //TODO use nonblocking read, wait for timeout with poll
        }
    }
private:
    inet_sock* sock_fd_;
};

}

#endif // _UNP_SOCK_STREAM_H_