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

/*
    reactor的个数应该与 thread 的个数相同，因为每个thread里会放一个reactor
    其中，处理listen事件的线程为第一线程，此线程会在 reactor上放 acceptor
*/

class tcp_server
{
public:
    using reactor_ptr_t	= Reactor*;
    using acceptor_ptr_t = std::shared_ptr<acceptor>;
    using pool_ptr_t = std::shared_ptr<thread::thread_pool>;
    static const int DEFAULT_THREAD_NUM = 4;
public:
    tcp_server(const net::inet_addr& local_addr);
    ~tcp_server();

public:
    //must call before open
    void set_reactor_type(reactor_imp_t_enum reactor_t) {reactor_t_ = reactor_t;}
    void set_thread_num(size_t n);

    int open();
    int close();

private:
    int make_reactor();
    void make_acceptor();

private:
    reactor_imp_t_enum 			reactor_t_ = reactor_imp_t_enum::USING_EPOLL;
    size_t						thread_num_ = DEFAULT_THREAD_NUM;

private:
    reactor_ptr_t				reactor_;
    acceptor_ptr_t 				acceptor_;
    pool_ptr_t          		pool_;
    net::inet_addr 				local_addr_;
};

} //namespace reactor
#endif // TCP_SERVER_H
