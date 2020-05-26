#include "inet_sock.h"
namespace net
{
inet_sock::inet_sock(int family, sock_type type, int protocol, bool reuse_addr)
  : fd()
  , family_(family)
  , type_(type)
  , protocol_(protocol)
  , reuse_addr_(reuse_addr)
{
  open();
}

inet_sock::inet_sock()
  : fd()
  , family_(0)
  , type_(sock_type::unknown)
  , protocol_(0)
  , reuse_addr_(false)
{ }

inet_sock::~inet_sock()
{
  if (canRead_ || canWrite_) {
    close(-1);
  }
}

int inet_sock::set_option(int level, int option, void *opt_val, socklen_t opt_len) const{
  return ::setsockopt(fd_, level, option, opt_val, opt_len);
}

int inet_sock::get_option(int level, int option, void *opt_val, socklen_t* opt_len) const{
  return ::getsockopt(fd_, level, option, opt_val, opt_len);
}

int net::inet_sock::open(){
  if (fd_ != INVALID_HANDLE && (canWrite_ || canRead_))
    close(-1);
  fd_ = ::socket(family_, (int)type_, protocol_);
  // LOG(INFO) << "opening a socket..." << handle_; 
  int one = 1;
  if(fd_ == INVALID_HANDLE) {
    return -1;
  } else if(reuse_addr_ && this->set_option(SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) == -1) {
    close(-1);
    return -1;
  }
  canWrite_ = true;
  canRead_ = true;
  return 0;
}

int inet_sock::close(int how){
  int ret = 0;
  if(this->fd_ != INVALID_HANDLE){
    // LOG(INFO) << "closing a socket..." << handle_;
    if (how == SHUT_RD || how == SHUT_WR)
      return shut_down(how);
    ret = ::close(fd_);
    this->fd_ = INVALID_HANDLE;
    canRead_ = false;
    canWrite_ = false;
  }
  return ret;
}

int inet_sock::shut_down(int how){
  int ret = 0;
  if(fd_ != INVALID_HANDLE){
    ret = ::shutdown(fd_, how);
    if(how == SHUT_RD)
      canRead_ = false;
    else if (how == SHUT_WR)
      canWrite_ = false;
  }
  return ret;
}
}
