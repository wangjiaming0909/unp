#include "inet_sock.h"
namespace net
{
int net::inet_sock::open(int family, sock_type type, int protocol, int reuse_addr){
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
    canWrite_ = true;
    canRead_ = true;
    return 0;
}

void net::inet_sock::set_handle(int handle)
{
    handle_ = handle;
    if(handle_ != INVALID_HANDLE)
    {
        canWrite_ = true;
        canRead_ = true;
    }
}

}