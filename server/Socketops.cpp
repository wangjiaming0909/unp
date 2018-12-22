#include "Socketops.h"
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "server/util/XString.h"
#include <unistd.h>


int server::socket::create_socket_nonblock_or_die(sa_family_t family){
    int fd = ::socket(family,
                      SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                      IPPROTO_TCP);
    if(fd < 0){
        LOG(ERROR) << "socket error ...";
        LOG(ERROR) << strerror(errno);
        exit(-1);
    }
    return fd;
}

void server::socket::bind_or_close(int listen_fd, const struct sockaddr_in* serv_addr){
    int ret = ::bind(listen_fd,
                     reinterpret_cast<const sockaddr*>(serv_addr),
                     static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    if(ret == -1){
        LOG(ERROR) << "bind error...";
        LOG(ERROR) << strerror(errno);
        exit(-1);
    }
}

void server::socket::listen(int listen_fd){
    int ret = ::listen(listen_fd, SOMAXCONN);
    if(ret == -1){
        LOG(ERROR) << "listen error...";
        LOG(ERROR) << strerror(errno);
    }
}

int server::socket::accept(int listen_fd, struct sockaddr_in* client_addr){
    socklen_t addr_len = static_cast<socklen_t>(sizeof *client_addr);
    int client_fd = ::accept4(listen_fd,
                              reinterpret_cast<struct sockaddr*>(client_addr),
                              &addr_len,
                              SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(client_fd == -1){
        LOG(ERROR) << "accept4 error...";
        LOG(ERROR) << strerror(errno);
    }
    return client_fd;
}

void server::socket::close(int sock_fd){
    if(::close(sock_fd) == -1){
        LOG(ERROR) << "close error...";
        LOG(ERROR) << strerror(errno);
    }
}


void server::socket::connect_or_close(int local_fd, const struct sockaddr_in* serv_addr){
    int ret = ::connect(local_fd,
                        reinterpret_cast<const struct sockaddr*>(serv_addr),
                        static_cast<socklen_t>(sizeof *serv_addr));
    if(ret == -1){
        LOG(ERROR) << "connect error...";
        LOG(ERROR) << strerror(errno);
    }
}


util::string&& server::socket::from_in_addr_to_string(const in_addr& addr){
    char buf[64];
    bzero(buf, 64);
    ::inet_ntop(AF_INET, &addr, buf, 64);
    return std::move(util::string(buf));
}

in_addr&& server::socket::from_string_to_in_addr(const util::string& addr_str){
    in_addr addr;
    int ret = ::inet_pton(AF_INET, addr_str.c_str(), &addr);
    if(ret != 0){
        LOG(ERROR) << "transation to in_addr failed";
        LOG(ERROR) << strerror(errno);
    }
    return std::move(addr);
}
