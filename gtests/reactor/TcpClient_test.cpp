#include <gtest/gtest.h>
#include <thread>
#include "reactor/tcp_client.h"
#include "reactor/connector.h"
#include "reactor/echo_client_handler.h"
#include "examples/HttpClientHandler.h"

TEST(tcp_client, normal)
{
    // GTEST_SKIP();
    using namespace reactor;
    using namespace std::chrono_literals; 

    tcp_client client{unp::reactor_imp_t_enum::USING_SELECT};
    // using Connector_t = connector<echo_client_handler>;
    using Connector_t = connector<examples::HttpClientHandler>;


    // net::inet_addr target_addr{22, "127.0.0.1"};
    // net::inet_addr target_addr{9090, "192.168.0.2"};
    net::inet_addr target_addr{80, "61.135.169.121"};


    std::vector<Connector_t*> conns{};

    char* url = "http://www.baidu.com";
    char* userAgent = "Mozilla/5.0 (X11; Ubuntu; Linu…) Gecko/20100101 Firefox/61.0";

    int connCount = 1;
    for(int i = 0; i < connCount; i++)
    {
        auto* conn = client.addConnection<Connector_t>(url, userAgent);
        ASSERT_TRUE(conn != nullptr);
        conns.push_back(conn);
        auto* handler = conn->connect(target_addr, 2s);
        if(handler == nullptr) FAIL();
    }

    std::thread th1{&tcp_client::start, &client};

    // std::this_thread::sleep_for(10s);
    // for(int i = 0; i < connCount; i++)
    // {
    //     conns[i]->disconnect(2s);
    // }
    // std::this_thread::sleep_for(4s);
    // client.suspend();

    th1.join();
}
