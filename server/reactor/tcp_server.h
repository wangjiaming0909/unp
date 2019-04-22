#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "server/reactor/acceptor.h"
#include "server/thread/thread_pool.h"
#include "server/thread/message_queue.h"
#include "server/net/addr.h"
#include "server/reactor/reactor.h"
#include <memory>

namespace reactor
{

enum class reactor_imp_t_enum
{
    USING_SELECT, USING_POLL, USING_EPOLL
};

class tcp_server
{
public:
    using reactor_ptr_t	= std::shared_ptr<Reactor>;
    using acceptor_ptr_t = std::shared_ptr<acceptor>;
public:
    tcp_server(const net::inet_addr& local_addr);

public:
    //must call before open
    void set_reactor_type(reactor_imp_t_enum reactor_t) {reactor_t_ = reactor_t;}
    void set_thread_num(size_t n) {thread_num_ = n;}

public:
    int open();
    int close();

private:
    reactor_imp_t_enum 			reactor_t_ = reactor_imp_t_enum::USING_EPOLL;
    size_t						thread_num_ = 8;

private:
    reactor_ptr_t				reactor_;
    acceptor_ptr_t 				acceptor_;
    thread::thread_pool			pool_;
    net::inet_addr 				local_addr_;
};

} //namespace reactor
#endif // TCP_SERVER_H
