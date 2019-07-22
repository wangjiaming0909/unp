#include "reactor/tcp_client.h"
#include "reactor/reactor.h"
#include "reactor/select_reactor_impl.h"
#include "reactor/poll_reactor_impl.h"
#include "reactor/epoll_reactor_impl.h"
#include "util/easylogging++.h"

namespace reactor
{

tcp_client::tcp_client() : reactor_() { }

tcp_client::~tcp_client() { }

int tcp_client::open(unp::reactor_imp_t_enum type)
{
    reactor_ = make_reactor(type);
    if(!reactor_) return -1;
    return 0;
}

int tcp_client::connect(const net::inet_addr& target_addr, const microseconds_t& timeout)
{
    // if(!connector_) return -1;

    // if(connector_->connect(target_addr, timeout) <= 0)
    // {
    //     LOG(WARNING) << "connector error...";
    //     return -1;
    // }
    // while(true)
    // {
    //     int ret = reactor_->handle_events();
    //     if(ret != 0)
    //     {
    //         LOG(WARNING) << "handle events error";
    //         return -1;
    //     }
    // }
    return 0;
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
