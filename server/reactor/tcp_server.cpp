#include "tcp_server.h"
#include "select_reactor_impl.h"
#include "poll_reactor_impl.h"
#include "util/easylogging++.h"

namespace reactor
{

tcp_server::tcp_server(const net::inet_addr& local_addr)
    : reactor_(0)
    , acceptor_()
    , pool_()
    , local_addr_(local_addr)
{
}

tcp_server::~tcp_server()
{
    acceptor_.reset();
    delete reactor_;
}

int tcp_server::open()
{
    if(make_reactor() != 0)
        return -1;

    make_acceptor();
    pool_ = std::make_shared<thread::thread_pool>(thread_num_);
}

int tcp_server::close()
{

}

int tcp_server::make_reactor()
{
    switch(reactor_t_)
    {
        case reactor_imp_t_enum::USING_SELECT:
            reactor_ = new Reactor(new select_reactor_impl{});
            break;
        case reactor_imp_t_enum::USING_POLL:
            reactor_ = new Reactor(new poll_reactor_impl{});
            break;
        case reactor_imp_t_enum::USING_EPOLL:
            reactor_ = new Reactor(new epoll_reactor_impl{});
            break;
        default:
            reactor_ = 0;
            return -1;
    }
    return 0;
}

void tcp_server::make_acceptor()
{
    acceptor_ = std::make_shared<acceptor>(*reactor_, local_addr_);
}

void tcp_server::set_thread_num(size_t n)
{
    if(n == 0 || n > 32)
    {
        LOG(WARNING) << "thread num can't be large than 32 or can't be 0: " << n;
        n = DEFAULT_THREAD_NUM;
    }

    thread_num_ = n;
}



} //namespace reactor
