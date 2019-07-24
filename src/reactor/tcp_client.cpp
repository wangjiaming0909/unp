#include "reactor/tcp_client.h"
#include "reactor/reactor.h"
#include "reactor/select_reactor_impl.h"
#include "reactor/poll_reactor_impl.h"
#include "reactor/epoll_reactor_impl.h"
#include "util/easylogging++.h"

namespace reactor
{

tcp_client::tcp_client() 
    : reactor_()
    // , manager_{*reactor_}
    , connectors_{}
    { }

tcp_client::~tcp_client() { }

int tcp_client::open(unp::reactor_imp_t_enum type)
{
    reactor_ = make_reactor(type);
    if(!reactor_) return -1;
    return 0;
}

void tcp_client::addConnector(IConnector& connector)
{

}

void tcp_client::closeConnector(IConnector& connector)
{

}

int tcp_client::start()
{

}

int tcp_client::suspend()
{

}

int tcp_client::stop()
{

}

tcp_client::reactor_ptr_t tcp_client::make_reactor(unp::reactor_imp_t_enum type)
{
    reactor_ptr_t reactor{};
    switch (type)
    {
    case unp::reactor_imp_t_enum::USING_SELECT:
        reactor = std::make_shared<Reactor>(new select_reactor_impl());
        break;
    case unp::reactor_imp_t_enum::USING_POLL:
        reactor = std::make_shared<Reactor>(new poll_reactor_impl());
        break;
    case unp::reactor_imp_t_enum::USING_EPOLL:
        reactor = std::make_shared<Reactor>(new epoll_reactor_impl());
        break;
    default:
        break;
    }
    return reactor;
}

} //namespace reactor
