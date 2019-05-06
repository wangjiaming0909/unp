#ifndef _UNP_SOCK_ACCEPTOR_H_
#define _UNP_SOCK_ACCEPTOR_H_

#include "inet_addr.h"
#include "sock_stream.h"
#include <boost/make_shared.hpp>

namespace net{
#define DEFAULT_BACKLOG 5

class sock_acceptor{
public:
    // sock_acceptor();
    sock_acceptor(const inet_addr& local_addr,
                int reuse_addr = 0,
                int protocol_family = AF_INET,
                int backlog = DEFAULT_BACKLOG,
                int protocol = 0);
    ~sock_acceptor();
    int open(const inet_addr& local_addr,
                int reuse_addr = 1,
                int protocol_family = AF_INET,
                int backlog = DEFAULT_BACKLOG,
                int protocol = 0);
    int close();
    int accept(sock_stream& new_stream,
               inet_addr* remote_addr = 0,
               microseconds* timeout = 0,
               bool restart = true) const;
    int get_handle() const;
protected:
    int shared_open(const inet_addr& local_sap,
                    int protocol_family,
                    int backlog);
    int shared_accept_start(microseconds *timeout,
                    bool restart,
                    int& in_blocking_mode) const;
    int shared_accept_finish(sock_stream& client_stream,
                    bool in_blocking_mode) const;
private:
    // inet_sock * sock_fd_;
    boost::shared_ptr<inet_sock> sock_fd_;

};
}
#endif // _UNP_SOCK_ACCEPTOR_H_