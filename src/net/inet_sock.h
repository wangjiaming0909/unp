#ifndef _UNP_SOCK_H_
#define _UNP_SOCK_H_

#include <sys/socket.h>
#include "unp.h"
#include "inet_addr.h"
#include "handle_set.h"
#include <unistd.h>
#include "util/easylogging++.h"
#include "net/fd.h"
namespace net{

enum class sock_type{
    stream = SOCK_STREAM,
    dgram = SOCK_DGRAM,
    seqpacket = SOCK_SEQPACKET,
    sock_raw = SOCK_RAW,
    unknown = 0
};

class inet_sock : public unp::fd{
public:
  inet_sock();
  inet_sock(int family, sock_type type, int protocol, bool reuse_addr = true);
  ~inet_sock();
  inline int set_option(int level, int option, void *opt_val, socklen_t opt_len) const;
  inline int get_option(int level, int option, void *opt_val, socklen_t* opt_len) const;
  virtual int open() override;
  virtual int close(int how) override;
  void set_sock_info(int family, sock_type type, int protocol, bool reuse_addr = true)
  {
    family_ = family;
    type_ = type;
    protocol_ = protocol;
    reuse_addr_ = reuse_addr;
  }
private:
  int shut_down(int how);
private:
  int family_;
  sock_type type_;
  int protocol_;
  bool reuse_addr_;
  inet_sock(const inet_sock&);
};  

}

#endif // _UNP_SOCK_H_
