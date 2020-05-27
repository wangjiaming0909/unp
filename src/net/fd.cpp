#include "net/fd.h"
#include <fcntl.h>
#include <sys/ioctl.h>

namespace unp
{

fd::fd() { }

fd::~fd() { }

int fd::fcntl(int cmd, long arg)
{
  return ::fcntl(fd_, cmd, arg);
}

int fd::ioctl(int cmd, void* arg)
{
  return ::ioctl(fd_, cmd, arg);
}

void fd::set_fd(int fd)
{
  fd_ = fd;
  canWrite_ = true;
  canRead_ = true;
}
int fd::set_non_blocking()
{
  if(fd_ == INVALID_HANDLE) return -1;
  // LOG(INFO) << "set socket to non-blocking mode..." << handle_;
  auto flags = fcntl(F_GETFL, 0);
  SET_BIT(flags, O_NONBLOCK);
  return fcntl(F_SETFL, flags);
}

int fd::restore_blocking()
{
  if(fd_ == INVALID_HANDLE) return -1;
  // LOG(INFO) << "restore socket to blcoking mode...";
  auto flags = fcntl(F_GETFL, 0);
  CLR_BIT(flags, O_NONBLOCK);
  return fcntl(F_SETFL, flags);
}

int fd::get_flags()
{
  return fcntl(F_GETFL, 0);
}
int fd::set_flags(int cmd, long arg)
{
  return fcntl(cmd, arg);
}
}
