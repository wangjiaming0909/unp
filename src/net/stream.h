#pragma once
#include "boost/noncopyable.hpp"
#include "net/fd.h"

namespace reactor
{

class buffer;
struct Stream : public boost::noncopyable
{
  virtual ~Stream() = default;
  virtual int open()
  {
    if (fd_)
      return fd_->open();
    return -1;
  }
  virtual int close()
  {
    if (fd_)
      return fd_->close(-1);
    return -1;
  }
  virtual ssize_t read(void *buffer, size_t len) = 0;
  virtual ssize_t read(reactor::buffer &buf, size_t len) = 0;
  virtual ssize_t write(const void *buffer, size_t len) = 0;
  virtual ssize_t send(const void *buffer, size_t len, int flags) = 0;
  virtual ssize_t send_n(const void *buffer, size_t len, int flags) = 0;
  virtual ssize_t writev_n(const void *buffer, size_t len) = 0;
  virtual int get_handle() const = 0;
  virtual int set_handle(int handle) = 0;
  virtual bool has_handle() const = 0;
  virtual int setNonBolcking() = 0;
  virtual int restoreBlocking() = 0;
  virtual void close_read() = 0;
  virtual void close_write() = 0;
  bool can_write() const
  {
    if(!fd_)
      return false;
    return fd_->canWrite();
  }
  bool can_read() const
  {
    if (!fd_)
      return false;
    return fd_->canRead();
  }

protected:
  unp::fd* fd_ = nullptr;
};
}
