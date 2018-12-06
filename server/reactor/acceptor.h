#ifndef _UNP_REACTOR_ACCEPTOR_H_
#define _UNP_REACTOR_ACCEPTOR_H_
#include "event_handler.h"
#include "../net/sock_acceptor.h"
#include "../net/inet_addr.h"

namespace reactor{
// enum class IO_TYPE{
//     SELECT, POLL, EPOLL
// };

enum {
    NOT_REUSE_ADDR = 0,
    REUSE_ADDR = 1
};

class reactor_acceptor : public event_handler{
public:
    //here use a inet_addr, so that we can specify an interface and a port to listen
    reactor_acceptor(const net::inet_addr& local_addr);  
    ~reactor_acceptor();
    virtual int handle_input(int handle) override;
    //accept do not need output
    // virtual int handle_output(int handle) override;
    virtual int handle_timeout(int handle) override;
    virtual int handle_close(int handle) override;
    virtual int handle_signal(int handle) override;
private:
    int get_handle() const ;
    int open();
    int close();
private:
    net::sock_acceptor	    acceptor_;
    net::inet_addr          local_addr_;
};
}
#endif // _UNP_REACTOR_ACCEPTOR_H_
