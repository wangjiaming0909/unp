#include "net/sock_stream.h"
#include <sys/uio.h>

namespace net
{

SockStream::SockStream(int family, int protocol)
{
  fd_ = new inet_sock(family, sock_type::stream, protocol);
  fd_->open();
}

ssize_t SockStream::read_imp(void *buffer, size_t len)
{
  auto handle = fd_->get_fd();
  if (handle == INVALID_HANDLE || !fd_->canRead())
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
  auto handle = fd_->get_fd();
  if (handle == INVALID_HANDLE || !fd_->canWrite())
    return -1;
  auto sendLen = ::send(handle, buffer, len, flags);
  //LOG(INFO) << "Send to: " << handle << " send: " << sendLen << "bytes";
  return sendLen;
}

ssize_t SockStream::readv_imp(iovec iov[], int n)
{
  auto handle = fd_->get_fd();
  if (handle == INVALID_HANDLE || !fd_->canRead())
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
  auto handle = fd_->get_fd();
  if (handle == INVALID_HANDLE || !fd_->canWrite())
    return -1;
  auto writeLen = ::writev(handle, iov, n);
  LOG(INFO) << "Send to: " << handle << " send: " << writeLen << "bytes";
  return writeLen;
}

int SockStream::set_handle(int handle)
{
  if(handle == INVALID_HANDLE)
    return -1;
  if (fd_ && fd_->get_fd() != INVALID_HANDLE)
    fd_->close(-1);
  if (!fd_)
    fd_ = new inet_sock();
  fd_->set_fd(handle);
  return handle;
}

void SockStream::set_sock_info(int family, int protocol, bool reuse_addr)
{
  if (!fd_ || fd_->get_fd() == INVALID_HANDLE) return;
  fd_->open();
  auto* sock_fd = dynamic_cast<inet_sock*>(fd_);
  if (!sock_fd) {
    LOG(ERROR) << "dynamic_cast error from fd to inet_sock";
    return;
  }
  sock_fd->set_sock_info(family, sock_type::stream, protocol, reuse_addr);
}

}
