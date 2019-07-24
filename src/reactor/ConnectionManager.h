#include "reactor/reactor.h"
#include "reactor/connector.h"
#include "boost/noncopyable.hpp"
#include "boost/intrusive/list.hpp"

#include <unordered_map>
#include <vector>

namespace reactor
{

template <typename Connector_t>
class ConnectionManager : boost::noncopyable
{
public:
    using BaseHandler_t = connection_handler<Connector_t>;
    using Container_t = boost::intrusive::list<connection_handler<Connector_t>, boost::intrusive::constant_time_size<false>>;

    ConnectionManager(Reactor& react);
    ~ConnectionManager();

    template <typename Handler_t, typename ...Args>
    Handler_t* makeConnection(Args&&... args);

    int closeConnection(BaseHandler_t* e, IConnector::micro_seconds timeout);

    Container_t::size_type connectionCount() const {return connectionCount_;}

private:
    Container_t connections_{};
    Reactor& reactor_;
    size_t connectionCount_{0};
};

template <typename Connector_t>
ConnectionManager<Connector_t>::ConnectionManager(Reactor& react) : reactor_(react)
{ }

template <typename Connector_t>
ConnectionManager<Connector_t>::~ConnectionManager()
{ }

template <typename Connector_t>
template <typename Handler_t, typename ...Args>
Handler_t* ConnectionManager<Connector_t>::makeConnection(Args&&... args)
{
    static_assert(std::is_base_of<BaseHandler_t, Handler_t>::value, "Handler_t should derive from connection_handler");
    auto* handler = new Handler_t(reactor_, std::forward<Args>(args)...);

    connections_.push_back(*conn);
    connectionCount_++;
    return handler;
}

template <typename Connector_t>
int ConnectionManager<Connector_t>::closeConnection(BaseHandler_t* e, IConnector::micro_seconds timeout)
{
    if(!e->is_linked()) return -1;
    e->disconnect(timeout);
    delete e;
    connectionCount_--;
    return 0;
}


}//namespace reactor