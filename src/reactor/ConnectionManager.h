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

    template <typename Connector_t, typename Handler_t, typename ...Args>
    ConnectorPtr makeConnection(Args&&... args);

    template <typename Handler_t>
    void closeConnection(ConnectorPtr e, IConnector::micro_seconds timeout);

    Container_t::size_type connectionCount() const {return connections_.size();}

private:
    Container_t connections_;
    Reactor& reactor_;
};

template <typename Connector_t, typename Handler_t, typename ...Args>
typename ConnectionManager::ConnectorPtr ConnectionManager::makeConnection(Args&&... args)
{
    static_assert(std::is_base_of<connection_handler, Handler_t>::value, "Handler_t should derive from connection_handler");
    auto* handler = new Handler_t(reactor_, std::forward<Args>(args)...);

    IConnector* conn = new Connector_t{reactor_, *handler};
    connections_.push_front(*conn);
    return connections_.begin();
}

template <typename Handler_t>
void ConnectionManager::closeConnection(ConnectorPtr e, IConnector::micro_seconds timeout)
{
    static_assert(std::is_base_of<connection_handler, Handler_t>::value, "Handler_t should derive from connection_handler");

    e->disconnect(timeout);
    // delete 
    connections_.erase(e);
}


}//namespace reactor