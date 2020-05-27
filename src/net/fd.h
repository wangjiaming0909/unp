#pragma once
#include "boost/core/noncopyable.hpp"
#include "net/macros.h"
#include <fcntl.h>
#include <boost/noncopyable.hpp>

namespace unp
{
struct fd : public boost::noncopyable
{
  fd();
  virtual ~fd();
  virtual int open() = 0;
  virtual int close(int how) = 0;

  int fcntl(int cmd, long arg);
  int ioctl(int cmd, void*);
  int set_non_blocking();
  int restore_blocking();
  int get_flags();
  int set_flags(int cmd, long arg);

  int get_fd() const {return fd_;}
  void set_fd(int fd);
  bool canWrite() const{return canWrite_;}
  bool canRead() const{return canRead_;}
protected:
  int fd_ = INVALID_HANDLE;
  bool canWrite_ = false;
  bool canRead_ = false;
};
}
