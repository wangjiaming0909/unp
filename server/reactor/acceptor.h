#ifndef _UNP_REACTOR_ACCEPTOR_H_
#define _UNP_REACTOR_ACCEPTOR_H_
#include "event_handler.h"
#include "server/net/sock_acceptor.h"
#include "server/net/inet_addr.h"
#include "reactor.h"

namespace reactor{
// enum class IO_TYPE{
//     SELECT, POLL, EPOLL
// };

enum {
    NOT_REUSE_ADDR = 0,
    REUSE_ADDR = 1
};

/*
    1, reactor_acceptor has a inet_addr to identify the interface and port of listen fd
    2, reactor_acceptor do not need a set_handle() function, because the sock_acceptor already has a handle in it
    3, reactor_acceptor do not need to override output, because an acceptor do not need to write anything
    4, reactor_acceptor need to create a event_handler of io_handler to handle the I/O events of the accepted fd
*/

class reactor_acceptor : public event_handler{
public:
    //here use a inet_addr,
    //so that we can specify an interface and a port to listen
    reactor_acceptor(Reactor* react, const net::inet_addr& local_addr);  
    ~reactor_acceptor() override ;
    virtual int handle_input(int handle) override;
    //accept do not need output
    // virtual int handle_output(int handle) override;
    virtual int handle_timeout(int handle) override;
    virtual int handle_close(int handle) override;
    virtual int handle_signal(int handle) override;
    virtual int get_handle() const {return acceptor_.get_handle();}
private:
    int open();
    int close();
private:
    net::sock_acceptor	    acceptor_;
    net::inet_addr          local_addr_;
};
}
#endif // _UNP_REACTOR_ACCEPTOR_H_
