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

FileFD::~FileFD() { }

int FileFD::open()
{
  if (file_path_.empty())
    return -1;
  fd_ = ::open(file_path_.c_str(), open_flags_);
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
  auto ret = ::close(fd_);
  fd_ = INVALID_HANDLE;
  canWrite_ = false;
  canRead_ = false;
  return ret;
}

}
