#include <gtest/gtest.h>
#include <thread>
#include "reactor/acceptor.h"
#include "reactor/connector.h"
#include "reactor/select_reactor_impl.h"


TEST(acceptor, normal_without_handlers)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}};
    net::inet_addr localAddr{9090, "127.0.0.1"};
    acceptor *acpt = new acceptor{react, localAddr};
    acpt->open();
    ASSERT_EQ(acpt->acceptorState_, REGISTERED);

    // acpt->close()
    acpt->destroy_acceptor();
    ASSERT_EQ(acpt->acceptorState_, ALL_CLOSED);
}



TEST(acceptor, normal_with_handlers){
    using namespace reactor;
    using namespace std::chrono_literals;

    Reactor react{new select_reactor_impl{}};

    // make acceptor
    net::inet_addr localAddr{9090, "127.0.0.1"};
    acceptor *acpt = new acceptor{react, localAddr};
    acpt->open();

    //make connector
    // connector<echo_connection_handler> conn{react};

    // std::thread acceptorThread{&Reactor::handle_events, &react, nullptr};

    // std::thread connectorThread{};

    // acceptorThread.join();
}