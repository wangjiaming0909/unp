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
    Reactor react{new select_reactor_impl{}};
    ConnectionManager manager{react};

    // auto* handler = manager.makeHandler<echo_connection_handler>();
    // ASSERT_TRUE(handler != nullptr);

    // auto* handler2 = manager.makeHandler<echo_client_handler>();
    // ASSERT_TRUE(handler2 != nullptr);

    // auto* handler3 = manager.makeHandler<FakeConnectionHandler>("name1");
    // ASSERT_TRUE(handler3 != nullptr);
    // ASSERT_EQ(state, 1);
    // ASSERT_EQ(handler3->name, "name1");
    // manager.closeHandler(*handler3);
    // ASSERT_EQ(state, 0);
}
