#ifndef _UNP_SOCK_H_
#define _UNP_SOCK_H_

#include <sys/socket.h>
#include "unp.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
namespace net{

enum class sock_type{
    stream = SOCK_STREAM,
    dgram = SOCK_DGRAM,
    seqpacket = SOCK_SEQPACKET,
    sock_raw = SOCK_RAW
};

class inet_sock{
public:
    inline inet_sock();
    inline inet_sock(sock_type type, int protocol);
    inline ~inet_sock();
    int get_handler() const {return handler_;}
    void set_handler(int handler) { handler_ = handler; }
	///fcntl 
    inline int fcntl(int cmd, long arg)const;
    inline int ioctl(int cmd, void*) const;
    inline int set_option(int level, int option, void *opt_val, socklen_t opt_len) const;
    inline int get_option(int level, int option, void *opt_val, socklen_t* opt_len) const;
    inline int open(sock_type type, int protocol);
    inline int close();
private:
    inet_sock(const inet_sock&);
    int handler_;
};  

}

inline net::inet_sock::inet_sock() : handler_(INVALID_HANDLER){ };

inline net::inet_sock::~inet_sock(){ close();}

inline net::inet_sock::inet_sock(sock_type type, int protocol){
    this->open(type, protocol);
}

inline int net::inet_sock::ioctl(int cmd, void* arg) const{
    return ::ioctl(this->handler_, cmd, arg);
}

inline int net::inet_sock::fcntl(int cmd, long arg)const{
    return ::fcntl(this->handler_, cmd, arg);
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

#endif // _UNP_SOCK_H_
