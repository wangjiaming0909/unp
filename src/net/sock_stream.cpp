#include "sock_stream.h"
#include <sys/uio.h>

namespace net
{
ssize_t SockStream::read_imp(void *buffer, size_t len)
{
  auto handle = sock_fd_.get_handle();
  if (handle == INVALID_HANDLE || !sock_fd_.canRead())
    return -1;
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

ssize_t SockStream::send_imp(const void *buffer, size_t len, int flags)
{
  auto handle = sock_fd_.get_handle();
  if (handle == INVALID_HANDLE || !sock_fd_.canWrite())
    return -1;
  auto sendLen = ::send(handle, buffer, len, flags);
  //LOG(INFO) << "Send to: " << handle << " send: " << sendLen << "bytes";
  return sendLen;
}

ssize_t SockStream::readv_imp(iovec iov[], int n)
{
  auto handle = sock_fd_.get_handle();
  if (handle == INVALID_HANDLE || !sock_fd_.canRead())
    return -1;
  int read_len = ::readv(handle, iov, n);
  if(read_len > 0) {
    LOG(INFO) << "Read from: " << handle << " read: " << read_len << "bytes";
  } else if(read_len == 0) {
    LOG(INFO) << "Read a EOF: " << handle;
  } else {
    LOG(INFO) << "Read from: " << handle << " error: " << strerror(errno);
  }
  return read_len;
}

ssize_t SockStream::writev_imp(const iovec iov[], int n)
{
  auto handle = sock_fd_.get_handle();
  if (handle == INVALID_HANDLE || !sock_fd_.canWrite())
    return -1;
  auto writeLen = ::writev(handle, iov, n);
  LOG(INFO) << "Send to: " << handle << " send: " << writeLen << "bytes";
  return writeLen;
}

int SockStream::set_handle(int handle)
{
  if(handle == INVALID_HANDLE)
    return -1;
  if (sock_fd_.get_handle() != INVALID_HANDLE)
    sock_fd_.close();
  sock_fd_.set_handle(handle);
  return handle;
}

}
