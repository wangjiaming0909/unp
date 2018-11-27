#ifndef _UNP_REACTOR_ACCEPTOR_H_
#define _UNP_REACTOR_ACCEPTOR_H_
#include "event_handler.h"
#include "../net/sock_acceptor.h"
#include "../net/inet_addr.h"

namespace reactor{
enum class IO_TYPE{
    SELECT, POLL, EPOLL
};

class reactor_acceptor : public event_handler{
public:
    reactor_acceptor(const net::inet_addr& local_addr, IO_TYPE io_type);  
    ~reactor_acceptor();
    int get_handle() const ;
    int open();
    int close();
private:
    IO_TYPE                 io_type_;
    net::sock_acceptor	    acceptor_;
    net::inet_addr          local_addr_;
};
}
#endif // _UNP_REACTOR_ACCEPTOR_H_