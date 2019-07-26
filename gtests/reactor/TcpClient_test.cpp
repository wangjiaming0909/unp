#include <gtest/gtest.h>
#include <thread>
#include "reactor/tcp_client.h"
#include "reactor/connector.h"
#include "reactor/echo_client_handler.h"

TEST(tcp_client, normal)
{
    // GTEST_SKIP();
    using namespace reactor;
    using namespace std::chrono_literals; 

    tcp_client client{unp::reactor_imp_t_enum::USING_SELECT};
    using Connector_t = connector<echo_client_handler>;


    // net::inet_addr target_addr{22, "127.0.0.1"};
    // net::inet_addr target_addr{9090, "192.168.0.2"};
    net::inet_addr target_addr{9090, "163.184.251.157"};


    std::vector<Connector_t*> conns{};
    int connCount = 5;
    for(int i = 0; i < connCount; i++)
    {
        auto* conn = client.addConnection<Connector_t>(i);
        ASSERT_TRUE(conn != nullptr);
        conns.push_back(conn);
        auto* handler = conn->connect(target_addr, 2s);
        if(handler == nullptr) FAIL();
    }

    std::thread th1{&tcp_client::start, &client};

    // std::this_thread::sleep_for(10s);
    std::this_thread::sleep_for(4s);
    for(int i = 0; i < connCount; i++)
    {
        conns[i]->disconnect(2s);
    }
    // client.suspend();
    client.stop();

    th1.join();
}
