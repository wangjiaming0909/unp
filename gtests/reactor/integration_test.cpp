#include <gtest/gtest.h>
#include <memory>
#include "reactor/tcp_server.h"
#include "reactor/tcp_client.h"

TEST(ServerClientIntergratonTest, normal)
{
    using namespace reactor;
    net::inet_addr localAddr{9090, "127.0.0.1"};
    tcp_server server{localAddr};
    ASSERT_EQ(server.local_addr_, localAddr);

    server.set_thread_num(4);
    ASSERT_EQ(server.thread_num_, 4);
    
    // server.open(unp::reactor_imp_t_enum::USING_SELECT);

    tcp_client client{};
    client.open(unp::reactor_imp_t_enum::USING_SELECT);
}

