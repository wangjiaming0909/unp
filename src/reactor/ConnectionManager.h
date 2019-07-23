#include "reactor/connection_handler.h"
#include "reactor/reactor.h"
#include "boost/noncopyable.hpp"

#include <unordered_map>
#include <vector>

namespace reactor
{

class ConnectionManager : boost::noncopyable
{
public:
    using HashMap_t = std::unordered_map<int, connection_handler*>;
    ConnectionManager(Reactor& react);
    ~ConnectionManager();

    template <typename Handler_t, typename ...Args>
    Handler_t* makeHandler(Args&&... args)
    {
        static_assert(std::is_base_of<connection_handler, Handler_t>::value, "Handler_t should derive from connection_handler");
        auto* handler = new Handler_t(reactor_, std::forward<Args>(args)...);
        handlersMap_.emplace(handler->get_handle(), handler);
        return handler;
    }

    template <typename Handler_t>
    void closeHandler(Handler_t& handler)
    {
        static_assert(std::is_base_of<connection_handler, Handler_t>::value, "Handler_t should derive from connection_handler");
        handlersMap_.erase(handler.get_handle());
        delete &handler;
    }

private:
    HashMap_t handlersMap_;
    Reactor& reactor_;
};

}//namespace reactor