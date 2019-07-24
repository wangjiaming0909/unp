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
    using Container_t = boost::intrusive::list<IConnector, boost::intrusive::constant_time_size<false>>;
    using ConnectorPtr = Container_t::iterator;
    ConnectionManager(Reactor& react);
    ~ConnectionManager();

    template <typename Connector_t, typename ...Args>
    Connector_t* makeConnection(Args&&... args);

    template <typename Connector_t>
    void closeConnection(Connector_t& e, IConnector::micro_seconds timeout);

    Container_t::size_type connectionCount() const {return connections_.size();}

private:
    Container_t connections_;
    Reactor& reactor_;
};

template <typename Connector_t, typename ...Args>
Connector_t* ConnectionManager::makeConnection(Args&&... args)
{
    using Handler_t = typename Connector_t::HandlerT;
    static_assert(std::is_base_of<connection_handler, Handler_t>::value, "Handler_t should derive from connection_handler");
    auto* handler = new Handler_t(reactor_, std::forward<Args>(args)...);

    Connector_t* ret = new Connector_t{reactor_, *handler};
    IConnector* conn = ret;
    connections_.push_front(*conn);
    return ret;
}

template <typename Connector_t>
void ConnectionManager::closeConnection(Connector_t& e, IConnector::micro_seconds timeout)
{
    using Handler_t = typename Connector_t::HandlerT;
    static_assert(std::is_base_of<connection_handler, Handler_t>::value, "Handler_t should derive from connection_handler");
    e.disconnect(timeout);
    delete &e;
}


}//namespace reactor