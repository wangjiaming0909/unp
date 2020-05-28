#include "net/file_stream.h"
#include "net/file_fd.h"
#include "util/easylogging++.h"
#include "reactor/buffer.h"
#include <stdexcept>

namespace reactor
{

FileStream::FileStream(const char* path, int flags)
{
  fd_ = new unp::FileFD(path, flags);
}

FileStream::~FileStream()
{
  delete fd_;
  fd_ = nullptr;
}

ssize_t FileStream::read(void *buffer, size_t len)
{
  if (!fd_ || fd_->get_fd() == INVALID_HANDLE)
    return 0;
  auto handle = fd_->get_fd();
  int read_len = ::read(handle, static_cast<char *>(buffer), len);
  if(read_len > 0) {
    //LOG(INFO) << "Read from: " << handle << " read: " << read_len << "bytes";
  } else if(read_len == 0) {
    LOG(INFO) << "Read a EOF: " << handle;
  } else {
    LOG(INFO) << "Read from: " << handle << " error: " << strerror(errno);
  }
  return read_len;
}

ssize_t FileStream::read(reactor::buffer &buf, size_t len)
{
  char* const data_p = static_cast<char*>(::calloc(len + 1, 1));
  int read_len = this->read(data_p, len);
  if(read_len > 0 )
  {
    auto data_appended = buf.append(data_p, read_len);
    if(data_appended != read_len)
    {
      // LOG(WARNING) << "Read data from socket len: " << read_len << " appended into buffer len: " << data_appended;
      free(data_p);
      return -1;
    }
  }
  free(data_p);
  return read_len;
}

ssize_t FileStream::write(const void *buffer, size_t len)
{
  if(!fd_ || fd_->get_fd() == INVALID_HANDLE) 
    return 0;
  int write_len = 0;
  write_len = ::write(fd_->get_fd(), buffer, len);
  // LOG(INFO) << "Send to: " << sock_fd_->get_handle() << " send: " << len << "bytes";
  return write_len;
}

ssize_t FileStream::writev_n(const void *buffer, size_t len)
{
  NOT_USED(buffer);
  NOT_USED(len);
  THROW_NOT_IMPLETED_EXCEPTION;
}

ssize_t FileStream::send(const void *buffer, size_t len, int flags)
{
  NOT_USED(buffer);
  NOT_USED(len);
  NOT_USED(flags);
  THROW_NOT_IMPLETED_EXCEPTION;
}

ssize_t FileStream::send_n(const void *buffer, size_t len, int flags)
{
  NOT_USED(buffer);
  NOT_USED(len);
  NOT_USED(flags);
  THROW_NOT_IMPLETED_EXCEPTION;
}

int FileStream::set_handle(int handle)
{
  if(handle == INVALID_HANDLE)
    return -1;
  if (fd_ && fd_->get_fd() != INVALID_HANDLE)
    fd_->close(-1);
  if (!fd_)
    fd_ = new unp::FileFD();
  fd_->set_fd(handle);
  return handle;
}

void FileStream::close_read()
{
  THROW_NOT_IMPLETED_EXCEPTION;
}

void FileStream::close_write()
{
  THROW_NOT_IMPLETED_EXCEPTION;
}

int FileStream::get_handle() const
{
  if (!fd_)
    return INVALID_HANDLE;
  return fd_->get_fd();
}

bool FileStream::has_handle() const
{
  if (!fd_)
    return false; return fd_->get_fd() != INVALID_HANDLE;
}

int FileStream::setNonBolcking()
{
  if (fd_)
    return fd_->set_non_blocking();
  return -1;
}

int FileStream::restoreBlocking()
{
  if (fd_)
    return fd_->restore_blocking();
  return -1;
}

void FileStream::set_file_stream_info(const char* file_path, int flags)
{
  if (!file_path){
    LOG(ERROR) << "File path empty";
    return;
  }
  unp::FileFD* fd = nullptr;
  if (!fd_)
    fd = new unp::FileFD(file_path, flags);
  fd_ = fd;
}

}
