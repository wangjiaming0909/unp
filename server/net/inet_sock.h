#ifndef _UNP_SOCK_H_
#define _UNP_SOCK_H_

#include <sys/socket.h>
#include "unp.h"
namespace net{

enum class sock_type{
    stream = SOCK_STREAM,
    dgram = SOCK_DGRAM,
    seqpacket = SOCK_SEQPACKET,
    sock_raw = SOCK_RAW
};

class inet_sock{
public:
    inet_sock();
    inet_sock(sock_type type, int protocol);
    ~inet_sock();
    int get_handler() const {return handler_;}
    void set_handler(int handler) { handler_ = handler; }
    int ioctl(int cmd, void*) const;
    int set_option(int level, int option, void *opt_val, socklen_t opt_len) const;
    int get_option(int level, int option, void *opt_val, socklen_t* opt_len) const;
    int open(sock_type type, int protocol);
    int close();
private:
    inet_sock(const inet_sock&);
    int handler_;
};  

}

#endif // _UNP_SOCK_H_