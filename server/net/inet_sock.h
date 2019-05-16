#ifndef _UNP_SOCK_H_
#define _UNP_SOCK_H_

#include <sys/socket.h>
#include "unp.h"
#include "inet_addr.h"
#include "handle_set.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "macros.h"
#include "server/util/easylogging++.h"
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
    int get_handle() const {return handle_;}
    void set_handle(int handle) { handle_ = handle; }
	///fcntl 
    inline int fcntl(int cmd, long arg)const;
    inline int ioctl(int cmd, void*) const;
    inline int set_option(int level, int option, void *opt_val, socklen_t opt_len) const;
    inline int get_option(int level, int option, void *opt_val, socklen_t* opt_len) const;
    inline int open(int family, sock_type type, int protocol, int reuse_addr = 1);
    inline int close();
	void shut_down(int how);
    int set_non_blocking() const;
    int restore_blocking() const;
    int get_flags() const;
    int set_flags(int cmd, long arg) const;
private:
    inet_sock(const inet_sock&);
    int handle_;
};  

}

inline net::inet_sock::inet_sock() : handle_(INVALID_HANDLE){ };

inline net::inet_sock::~inet_sock(){ close();}

inline net::inet_sock::inet_sock(sock_type type, int protocol){
    this->open(AF_INET, type, protocol);
}

inline int net::inet_sock::ioctl(int cmd, void* arg) const{
    return ::ioctl(this->handle_, cmd, arg);
}

inline int net::inet_sock::fcntl(int cmd, long arg)const{
    return ::fcntl(this->handle_, cmd, arg);
}

inline int net::inet_sock::set_option(int level, int option, void *opt_val, socklen_t opt_len) const{
    return ::setsockopt(this->handle_, level, option, opt_val, opt_len);
}

inline int net::inet_sock::get_option(int level, int option, void *opt_val, socklen_t* opt_len) const{
    return ::getsockopt(this->handle_, level, option, opt_val, opt_len);
}

inline int net::inet_sock::open(int family, sock_type type, int protocol, int reuse_addr){
    this->handle_ = ::socket(family, (int)type, protocol);
    // LOG(INFO) << "opening a socket..." << handle_; 
	int one = 1;
	if(handle_ == INVALID_HANDLE){
		return -1;
	} else if( reuse_addr && 
			this->set_option(SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) == -1){
		close();
		return -1;
   }
   return 0;
}

inline int net::inet_sock::close(){
    int ret = 0;
    if(this->handle_ != INVALID_HANDLE){
        // LOG(INFO) << "closing a socket..." << handle_;
        ret = ::close(this->handle_);
        this->handle_ = INVALID_HANDLE;
    }
    return ret;
}

inline void net::inet_sock::shut_down(int how){
	if(handle_ != INVALID_HANDLE){
        // LOG(INFO) << "shutdown a socket..." << handle_;
		::shutdown(handle_, how);
	}
}

inline int net::inet_sock::set_non_blocking() const{
    if(handle_ == INVALID_HANDLE) return -1;
    // LOG(INFO) << "set socket to non-blocking mode..." << handle_;
    auto flags = fcntl(F_GETFL, 0);
    SET_BIT(flags, O_NONBLOCK);
    return fcntl(F_SETFL, flags);
}

inline int net::inet_sock::restore_blocking() const{
    if(handle_ == INVALID_HANDLE) return -1;
    // LOG(INFO) << "restore socket to blcoking mode...";
    auto flags = fcntl(F_GETFL, 0);
    CLR_BIT(flags, O_NONBLOCK);
    return fcntl(F_SETFL, flags);
}

inline int net::inet_sock::get_flags() const{
    return fcntl(F_GETFL, 0);
}

inline int net::inet_sock::set_flags(int cmd, long arg) const{
    return fcntl(cmd, arg);
}

#endif // _UNP_SOCK_H_
