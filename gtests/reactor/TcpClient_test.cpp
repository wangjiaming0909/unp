#include <gtest/gtest.h>
#include "reactor/tcp_client.h"
#include "reactor/connector.h"
#include "reactor/echo_client_handler.h"

TEST(tcp_client, normal)
{
    using namespace reactor;
    using namespace std::chrono_literals; 

    tcp_client client{unp::reactor_imp_t_enum::USING_SELECT};
    using Connector_t = connector<echo_client_handler>;

    auto* conn = client.addConnection<Connector_t>();
    ASSERT_TRUE(conn != nullptr);

    net::inet_addr target_addr{22, "127.0.0.1"};
    // if(conn->connect(target_addr, 2s) < 0) return;
    // client.start();
}
