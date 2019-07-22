#include <gtest/gtest.h>
#include "reactor/acceptor.h"
#include "reactor/select_reactor_impl.h"



TEST(acceptor, normal)
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
