#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include "net/inet_addr.h"
#include "net/unp.h"
#include "reactor/connector.h"
#include <boost/noncopyable.hpp>
#include "reactor/ConnectionManager.h"

#include <chrono>
#include <memory>

class Reactor;

namespace reactor
{

class tcp_client : boost::noncopyable
{
public:
    using reactor_ptr_t = Reactor*;
    using microseconds_t = std::chrono::microseconds;
    tcp_client(unp::reactor_imp_t_enum type);
    ~tcp_client();
    int open(unp::reactor_imp_t_enum type);
    template <typename Connector_t, typename ...Args>
    Connector_t* addConnection(Args&&... args);
    template <typename Connector_t>
    void closeConnection(Connector_t& connector, microseconds_t timeout);
    int start();
    int suspend();
    int stop();

private:
    reactor_ptr_t make_reactor(unp::reactor_imp_t_enum type);

private:
    // std::vector<IConnector*> connectors_;
    ConnectionManager* manager_{nullptr};
    reactor_ptr_t       reactor_{nullptr};
};

template <typename Connector_t, typename ...Args>
Connector_t* tcp_client::addConnection(Args&&... args)
{
    return manager_->makeConnection<Connector_t>(std::forward<Args>(args)...);
}

template <typename Connector_t>
void tcp_client::closeConnection(Connector_t& connector, microseconds timeout)
{
    manager_->closeConnection<Connector_t>(connector, timeout);
}


} //namespace reactor
#endif // TCP_CLIENT_H
