#include "socketops.h"
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "./util/XString.h"
#include <unistd.h>


int server::socket::create_socket_nonblock_or_die(sa_family_t family){
    int fd = ::socket(family,
                      SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                      IPPROTO_TCP);
    if(fd < 0){
        CONSOLE_LOG("socket error: " << strerror(errno));
        exit(-1);
    }
    return fd;
}

void server::socket::bind_or_close(int listen_fd, const struct sockaddr_in* serv_addr){
    int ret = ::bind(listen_fd,
                     reinterpret_cast<const sockaddr*>(serv_addr),
                     static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    if(ret == -1){
        CONSOLE_LOG("bind error: " << strerror(errno));
        exit(-1);
    }
}

void server::socket::listen(int listen_fd){
    int ret = ::listen(listen_fd, SOMAXCONN);
    if(ret == -1){
        CONSOLE_LOG("listen error: " << strerror(errno));
    }
}

int server::socket::accept(int listen_fd, struct sockaddr_in* client_addr){
    socklen_t addr_len = static_cast<socklen_t>(sizeof *client_addr);
    int client_fd = ::accept4(listen_fd,
                              reinterpret_cast<struct sockaddr*>(client_addr),
                              &addr_len,
                              SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(client_fd == -1){
        CONSOLE_LOG("accept4 errno: " << strerror(errno));
        exit(-1);
    }
    return client_fd;
}

void server::socket::close(int sock_fd){
    if(::close(sock_fd) == -1){
        CONSOLE_LOG("close error: " << strerror(errno));
    }
}


void server::socket::connect_or_close(int local_fd, const struct sockaddr_in* serv_addr){
    int ret = ::connect(local_fd,
                        reinterpret_cast<const struct sockaddr*>(serv_addr),
                        static_cast<socklen_t>(sizeof *serv_addr));
    if(ret == -1){
        CONSOLE_LOG("connect error: " << strerror(errno));
        exit(-1);//TODO need to handle the error
    }
}

//void server::socket::connect_or_close()

