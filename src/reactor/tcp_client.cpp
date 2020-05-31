#include "reactor/tcp_client.h"
#include "reactor/reactor.h"
#include "reactor/select_reactor_impl.h"
#include "reactor/poll_reactor_impl.h"
#include "reactor/epoll_reactor_impl.h"
#include "util/easylogging++.h"

namespace reactor
{

tcp_client::tcp_client(unp::reactor_imp_t_enum type)
{
    make_reactor(type);
    manager_ = new ConnectionManager(*reactor_);
}

tcp_client::~tcp_client()
{
    delete manager_;
    delete reactor_;
}

int tcp_client::open(unp::reactor_imp_t_enum type)
{
    reactor_ = make_reactor(type);
    if(!reactor_) return -1;
    return 0;
}

int tcp_client::start()
{
    reactor_->start();
    int ret = 0;
    while(ret >= 0)
    {
        ret = reactor_->handle_events();
    }
    return ret;
}

int tcp_client::suspend()
{
    reactor_->suspend();
    return 0;
}

int tcp_client::stop()
{
    suspend();
    //TODO wait until the loop is over, then close all connections
    return manager_->closeAllConnection();
}

tcp_client::reactor_ptr_t tcp_client::make_reactor(unp::reactor_imp_t_enum type)
{
    switch (type)
    {
    case unp::reactor_imp_t_enum::USING_SELECT:
        reactor_ = new Reactor(new select_reactor_impl(), true);
        break;
    case unp::reactor_imp_t_enum::USING_POLL:
        reactor_ = new Reactor(new poll_reactor_impl(), true);
        break;
    case unp::reactor_imp_t_enum::USING_EPOLL:
        reactor_ = new Reactor(new epoll_reactor_impl(), true);
        break;
    default:
        break;
    }
    return reactor_;
}

} //namespace reactor
