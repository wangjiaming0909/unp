#ifndef _UNP_SOCK_STREAM_H_
#define _UNP_SOCK_STREAM_H_
#include <chrono>
#include <sys/socket.h>
#include "inet_sock.h"
#include <unistd.h>
#include "unp.h"
#include "macros.h"
namespace net{

class sock_stream{
public:
    using micro_seconds = std::chrono::microseconds; 
    ssize_t recv( void* buffer, size_t len, 
        const micro_seconds* timeout = 0) const;
    ssize_t recvv( iovec iov[], int n,
        const micro_seconds* timeout = 0)const;
    ssize_t send( const void* buffer, size_t len, int flags,
        const micro_seconds* timeout = 0)const;
    ssize_t sendv( const iovec iov[], int n,
        const micro_seconds* timeout = 0) const;

private:
    // ssize_t nonblocking_recv()

private:
    ssize_t recv_imp(void* buffer, size_t len,
        const micro_seconds* timeout = 0) const;
	ssize_t send_imp(const void* buffer, size_t len, int flags,
		const micro_seconds* timeout = 0)const;
private:
    inet_sock* sock_fd_;
};

}

#endif // _UNP_SOCK_STREAM_H_
