#ifndef _UNP_SOCK_ACCEPTOR_H_
#define _UNP_SOCK_ACCEPTOR_H_

#include "inet_addr.h"
#include "inet_sock.h"
#include <memory>
#include <chrono>

namespace net{
#define DEFAULT_BACKLOG 5

class SockStream;
using namespace std::chrono_literals;

class sock_acceptor{
public:
  using microseconds = std::chrono::microseconds;
  // sock_acceptor();
  sock_acceptor(const inet_addr& local_addr, int reuse_addr = 0, 
      int protocol_family = AF_INET, int backlog = DEFAULT_BACKLOG,
      int protocol = 0, sock_type type = sock_type::stream);
  ~sock_acceptor();
  int open();
  int close();
  int accept(SockStream& new_stream, inet_addr* remote_addr = 0,
      microseconds timeout = 0s, bool restart = true) const;
  int get_handle() const { return sock_fd_->get_fd(); }

protected:
  int shared_open(const inet_addr& local_sap,
      int protocol_family,
      int backlog);
  int shared_accept_start(microseconds timeout, bool restart) const;
private:
  // inet_sock * sock_fd_;
  std::unique_ptr<inet_sock> sock_fd_;
  inet_addr local_addr_;
  int protocol_family_;
  int protocol_;
  sock_type sock_type_;
  int backlog_;
  bool reuse_addr_;
};
}
#endif // _UNP_SOCK_ACCEPTOR_H_
