#pragma once
#include "inet_sock.h"
#include "reactor/buffer.h"
#include "boost/noncopyable.hpp"
#include "net/stream.h"

#include <chrono>

namespace net
{

class SockStream : public reactor::Stream{
public:
  SockStream() {}
	SockStream(int family, int protocol);
  virtual ~SockStream() {delete fd_; fd_ = nullptr;}

public:
  virtual int open() override { return fd_->open(); }
  virtual int close() override { return fd_->close(-1); }
  //recv version use the system call recv which has a flags parameter
  virtual ssize_t recv(void *buffer, size_t len, int flags) = 0;
  virtual ssize_t readv(iovec iov[], int n) = 0;
  //send alse has a flags parameter
  virtual ssize_t writev(const iovec iov[], int n) = 0;
  virtual ssize_t recv_n(void *buffer, size_t len, int flags) = 0;
  virtual ssize_t readv_n(iovec iov[], size_t n) = 0;
  virtual int connect() { return 0; }
  virtual int accept() { return 0; }
  void set_sock_info(int family, int protocol, bool reuse_addr = true);

public:
  virtual void close_read() override
  { 
    if (fd_)
      fd_->close(SHUT_RD); 
  }
  virtual void close_write() override
  { 
    if (fd_)
      fd_->close(SHUT_WR); 
  }
  virtual int set_handle(int handle) override;
  virtual int get_handle() const override 
  { 
    if (fd_)
      return fd_->get_fd(); 
    return INVALID_HANDLE;
  }
  virtual bool has_handle() const override 
  { 
    if (!fd_)
      return false;
    return fd_->get_fd() != INVALID_HANDLE; 
  }
  int setNonBolcking() override 
  { 
    if (!fd_)
      return -1;
    return fd_->set_non_blocking(); 
  }
  int restoreBlocking() override 
  { 
    if (!fd_)
      return -1;
    return fd_->restore_blocking(); 
  }

protected:
  ssize_t read_imp(void *buffer, size_t len);
  ssize_t send_imp(const void* buffer, size_t len, int flags);
  ssize_t readv_imp(iovec iov[], int n);
  ssize_t writev_imp(const iovec iov[], int n);
};

}
