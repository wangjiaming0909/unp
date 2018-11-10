#include "inet_sock.h"
#include <sys/ioctl.h>
#include <unistd.h>

inline net::inet_sock::inet_sock() : handler_(INVALID_HANDLER){ };

inline net::inet_sock::~inet_sock(){ close();}

inline net::inet_sock::inet_sock(sock_type type, int protocol){
    this->open(type, protocol);
}

inline int net::inet_sock::ioctl(int cmd, void* arg) const{
    return ::ioctl(this->handler_, cmd, arg);
}
inline int net::inet_sock::set_option(int level, int option, void *opt_val, socklen_t opt_len) const{
    return ::setsockopt(this->handler_, level, option, opt_val, opt_len);
}

inline int net::inet_sock::get_option(int level, int option, void *opt_val, socklen_t* opt_len) const{
    return ::getsockopt(this->handler_, level, option, opt_val, opt_len);
}

inline int net::inet_sock::open(sock_type type, int protocol){
    this->handler_ = ::socket(AF_INET, (int)type, protocol);
    return this->handler_;
}
inline int net::inet_sock::close(){
    int ret = 0;
    if(this->handler_ != INVALID_HANDLER){
        ret = ::close(this->handler_);
        this->handler_ = INVALID_HANDLER;
    }
    return ret;
}