#include <gtest/gtest.h>
#include <string>
#include "reactor/ConnectionManager.h"
#include "reactor/reactor.h"
#include "reactor/select_reactor_impl.h"
#include "reactor/echo_connection_handler.h"
#include "reactor/echo_client_handler.h"

static int state = 0;

struct FakeConnectionHandler : public reactor::sock_connection_handler
{
public:
    FakeConnectionHandler(reactor::Reactor& react, const std::string& name) 
        : sock_connection_handler(react)
        , name(name)
        {
            state = 1;
        }
    ~FakeConnectionHandler()
    {
        state = 0;
    }

    std::string name;
};


TEST(ConnectionManager, makeHandler)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}};
    ConnectionManager manager{react};

    using Connector_t = connector<echo_connection_handler>;

    auto* conn1 = manager.makeConnection<Connector_t>();
    ASSERT_TRUE(conn1 != nullptr);

    // auto* handler2 = manager.makeHandler<echo_client_handler>();
    // ASSERT_TRUE(handler2 != nullptr);

    using Connector_t2 = connector<FakeConnectionHandler>;
    auto* conn3 = manager.makeConnection<Connector_t2>("name1");
    ASSERT_TRUE(conn3 != nullptr);
    ASSERT_EQ(state, 1);
    ASSERT_EQ(conn3->handlerPtr_->name, "name1");
    using namespace std::chrono_literals;
    manager.closeConnection<Connector_t2>(*conn3, 2s);
    ASSERT_EQ(state, 0);
}
