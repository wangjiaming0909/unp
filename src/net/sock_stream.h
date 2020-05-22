#pragma once
#include "inet_sock.h"
#include "reactor/buffer.h"
#include "boost/noncopyable.hpp"
#include <chrono>
#include "net/stream.h"

namespace net
{

class SockStream : public reactor::Stream{
public:
	SockStream() : sock_fd_{}{}

public:
  virtual void open() override {}
  void close() override { sock_fd_.close(); }
  //recv version use the system call recv which has a flags parameter
  virtual ssize_t recv(void *buffer, size_t len, int flags) = 0;
  virtual ssize_t readv(iovec iov[], int n) = 0;
  //send alse has a flags parameter
  virtual ssize_t writev(const iovec iov[], int n) = 0;
  virtual ssize_t recv_n(void *buffer, size_t len, int flags) = 0;
  virtual ssize_t readv_n(iovec iov[], size_t n) = 0;
  virtual int connect() { return 0; }
  virtual int accept() { return 0; }
  //when connecting, open a sockfd first
  virtual int openSockFD(int family, sock_type type, int protocol, int reuse_addr)
  {
    return sock_fd_.open(family, type, protocol, reuse_addr);
  }
  //when accepting, the fd will be open by ::accept, so set it into the sockstream
  virtual int set_handle(int handle) override;

public:
  virtual void close_read() override { sock_fd_.shut_down(SHUT_RD); }
  virtual void close_write() override { sock_fd_.shut_down(SHUT_WR); }
  int getHandle() const override { return sock_fd_.get_handle(); }
  //[handle] must be a opened socket handle
  bool hasHandle() const override { return sock_fd_.get_handle() != INVALID_HANDLE; }
  int setNonBolcking() override { return sock_fd_.set_non_blocking(); }
  int restoreBlocking() override { return sock_fd_.restore_blocking(); }

protected:
  ssize_t read_imp(void *buffer, size_t len);
  ssize_t send_imp(const void* buffer, size_t len, int flags);
  ssize_t readv_imp(iovec iov[], int n);
  ssize_t writev_imp(const iovec iov[], int n);

protected:
  inet_sock sock_fd_;
};

}
