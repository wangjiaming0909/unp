#include <gtest/gtest.h>
#include <string>
#include "reactor/ConnectionManager.h"
#include "reactor/reactor.h"
#include "reactor/select_reactor_impl.h"
#include "reactor/echo_connection_handler.h"
#include "reactor/echo_client_handler.h"

static int state = 0;

struct FakeConnectionHandler : public reactor::connection_handler
{
public:
    FakeConnectionHandler(reactor::Reactor& react, const std::string& name) 
        : connection_handler(react)
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
    using namespace std::chrono_literals;
    Reactor react{new select_reactor_impl{}};
    ConnectionManager manager{react};

    auto* connector1 = manager.makeConnection<connector, echo_connection_handler>();
    ASSERT_TRUE(connector1 != nullptr);

    auto* connector2 = manager.makeConnection<connector, echo_client_handler>();
    ASSERT_TRUE(connector2 != nullptr);

    auto* connector3 = manager.makeConnection<connector, FakeConnectionHandler>("name1");
    ASSERT_TRUE(connector3 != nullptr);
    ASSERT_EQ(state, 1);
    // ASSERT_EQ(connector3->handlerPtr_->, );
    manager.closeConnection(connector3, 200000us);
    ASSERT_EQ(state, 0);
}
