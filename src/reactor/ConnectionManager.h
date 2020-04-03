#include "reactor/reactor.h"
#include "reactor/connector.h"
#include "boost/noncopyable.hpp"
#include "boost/intrusive/list.hpp"

#include <unordered_map>
#include <vector>

namespace reactor
{

class ConnectionManager : boost::noncopyable
{
public:
    using Container_t = boost::intrusive::list<ServiceT, boost::intrusive::constant_time_size<false>>;
    using ConnectorPtr = Container_t::iterator;
    ConnectionManager(Reactor& react);
    ~ConnectionManager();

    template <typename ServiceImpl, typename ...Args>
    ServiceImpl* makeConnection(Args&&... args);

    template <typename ServiceImpl>
    void closeConnection(ServiceImpl& e, std::chrono::microseconds timeout);
    int closeAllConnection();

    Container_t::size_type connectionCount() const {return connectionCount_;}

private:
    Reactor& reactor_;
    Container_t connections_;
    size_t connectionCount_{0};
};

template <typename ServiceImpl, typename ...Args>
ServiceImpl* ConnectionManager::makeConnection(Args&&... args)
{
    using Handler_t = typename ServiceImpl::HandlerT;
    static_assert(std::is_base_of<connection_handler, Handler_t>::value, "Handler_t should derive from connection_handler");
    static_assert(std::is_base_of<ServiceT, ServiceImpl>::value, "ServiceImpl should drive from ServiceT");
    auto* handler = new Handler_t(reactor_, std::forward<Args>(args)...);

    ServiceImpl* ret = new ServiceImpl{*handler};
    ServiceT* conn = ret;
    connections_.push_front(*conn);
    connectionCount_++;
    return ret;
}

template <typename ServiceImpl>
void ConnectionManager::closeConnection(ServiceImpl& e, std::chrono::microseconds timeout)
{
    using Handler_t = typename ServiceImpl::HandlerT;
    static_assert(std::is_base_of<connection_handler, Handler_t>::value, "Handler_t should derive from connection_handler");
    static_assert(std::is_base_of<ServiceT, ServiceImpl>::value, "ServiceImpl should drive from ServiceT");
    e.disconnect(timeout);
    delete &e;
    connectionCount_--;
}


}//namespace reactor
