#ifndef _UNP_SOCK_STREAM_H_
#define _UNP_SOCK_STREAM_H_
#include <chrono>
#include <sys/socket.h>
#include <unistd.h>

#include "inet_sock.h"
#include "unp.h"
#include "macros.h"
#include "util/easylogging++.h"
#include "sock_stream.h"
namespace net{

//InetSockStream does not contain the memory of the sock_fd
//when need to use non-blocking read and write 
//need to change the status of the fd first
//then use InetSockStream
class InetSockStream : public SockStream{
public:
	InetSockStream() : SockStream{}{}
    ~InetSockStream() = default;

public:
    //read version use system call read which has no flags
    ssize_t read(void *buffer, size_t len) override;
    ssize_t read(reactor::buffer &buf, size_t len) override;
    ssize_t write(const void *buffer, size_t len) override;
    //recv version use the system call recv which has a flags parameter

    ssize_t readv(iovec iov[], int n) override;
    //send alse has a flags parameter
    ssize_t send(const void *buffer, size_t len, int flags) override;
    ssize_t writev(const iovec iov[], int n) override;

    ssize_t recv(void *buffer, size_t len, int flags) override;
    ssize_t recv_n(void *buffer, size_t len, int flags) override;
    ssize_t readv_n(iovec iov[], size_t n) override;
    ssize_t send_n(const void *buffer, size_t len, int flags) override;
    ssize_t writev_n(const void *buffer, size_t len) override;
};
}

#endif // _UNP_SOCK_STREAM_H_
