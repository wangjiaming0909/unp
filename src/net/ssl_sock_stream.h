#pragma once
#include <openssl/ssl.h>
#include "sock_stream.h"

namespace net
{

class SSLSockStream : public SockStream
{
public:
    //read version use system call read which has no flags
    ssize_t read(void *buffer, size_t len) override;
    ssize_t read(reactor::buffer &buf, size_t len) override;
    ssize_t write(const void *buffer, size_t len) override;
    //recv version use the system call recv which has a flags parameter

    ssize_t recv(void *buffer, size_t len, int flags) override;
    ssize_t readv(iovec iov[], int n) override;
    //send alse has a flags parameter
    ssize_t send(const void *buffer, size_t len, int flags) override;
    ssize_t writev(const iovec iov[], int n) override;

    ssize_t recv_n(void *buffer, size_t len, int flags) override;
    ssize_t readv_n(iovec iov[], size_t n) override;
    ssize_t send_n(const void *buffer, size_t len, int flags) override;
    ssize_t writev_n(const void *buffer, size_t len) override;
};

}