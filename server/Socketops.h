#ifndef SOCKETOPS_H
#define SOCKETOPS_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "util/XString.h"
#include "util/easylogging++.h"

namespace server {
namespace socket {
//tcp+ipv4 server
    int create_socket_nonblock_or_die(sa_family_t family);
    void bind_or_close(int listen_fd, const struct sockaddr_in* serv_addr);
    void listen(int listen_fd);
    int accept(int listen_fd, struct sockaddr_in* client_addr);
    void close(int sock_fd);
//client
    void connect_or_close(int local_fd, const struct sockaddr_in* serv_addr);//no ipv6
//address transation
    util::string&& from_in_addr_to_string(const in_addr& addr);
    in_addr&& from_string_to_in_addr(const util::string& addr_str);
}
}

#endif // SOCKETOPS_H
