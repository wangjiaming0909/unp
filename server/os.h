#ifndef _UNP_OS_H_
#define _UNP_OS_H_
#include <netinet/in.h>
#include <unistd.h>
#include <sys/ioctl.h>

namespace os{

class Ios{
public:
    virtual int socket(int family, int type, int protocol) = 0;
    virtual int bind(int listen_fd, const sockaddr* addr, socklen_t len) = 0;
    virtual int listen(int listen_fd, int n) = 0;
    virtual int close(int fd) = 0;
    virtual int getsockopt(int sock_fd, int level, int option_name, void* opt_val, socklen_t* opt_len) = 0;
    virtual int setsockopt(int sock_fd, int level, int option_name, void* opt_val, socklen_t opt_len) = 0;
    virtual int ioctl(int fd, int cmd, void* arg) = 0;
};

class os : public Ios{
public:
    int socket(int family, int type, int protocol) override{
        return ::socket(family, type, protocol);
    }
    int bind(int listen_fd, const sockaddr *addr, socklen_t len) override{
        return ::bind(listen_fd, addr, len);
    }
    int listen(int listen_fd, int n) override{
        return ::listen(listen_fd, n);
    }
    int close(int fd) override{
        return ::close(fd);
    }
    int getsockopt(int sock_fd, int level, int option_name, void *opt_val, socklen_t* opt_len) override{
        return ::getsockopt(sock_fd, level, option_name, opt_val, opt_len);
    }
    int setsockopt(int sock_fd, int level, int option_name, void *opt_val, socklen_t opt_len) override{
        return ::setsockopt(sock_fd, level, option_name, opt_val, opt_len);
    }
    int ioctl(int fd, int cmd, void *arg) override{
        return ::ioctl(fd, cmd, arg);
    }
};
}
#endif // _UNP_OS_H_
