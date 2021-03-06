#include "net/file_fd.h"
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

namespace unp
{
FileFD::FileFD()
{
}

FileFD::FileFD(const char* file_path, int flags)
  : file_path_(file_path)
  , open_flags_(flags)
{ }

FileFD::~FileFD()
{
  if (fd_ != INVALID_HANDLE) {
    close(-1);
  }
}

int FileFD::open()
{
  if (file_path_.empty())
    return -1;
  fd_ = ::open(file_path_.c_str(), open_flags_);
  LOG(DEBUG) << "opening a file: " << file_path_ << " fd: " << fd_;
  if (open_flags_ & O_RDONLY) {
    canRead_ = true;
  }
  if (open_flags_ & O_WRONLY) {
    canWrite_ = true;
  }
  if (open_flags_ & O_RDWR) {
    canWrite_ = true;
    canRead_ = true;
  }
  return fd_;
}

int FileFD::close(int /*how*/)
{
  LOG(DEBUG) << "closing a fd: " << fd_;
  if (fd_ == INVALID_HANDLE) {
    return -1;
  }
  auto ret = ::close(fd_);
  fd_ = INVALID_HANDLE;
  canWrite_ = false;
  canRead_ = false;
  return ret;
}

}
