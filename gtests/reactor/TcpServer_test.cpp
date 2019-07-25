#include <gtest/gtest.h>
#include "reactor/tcp_server.h"

TEST(TcpServer, normal)
{
    GTEST_SKIP();
    using namespace reactor;
    using namespace std::chrono_literals;

    net::inet_addr listenAddr{9090, "127.0.0.1"};
    tcp_server server{listenAddr};

    server.set_thread_num(4);

    std::thread th1{&tcp_server::start, &server, unp::reactor_imp_t_enum::USING_SELECT};
    // server.suspend();

    // server.stop();
    th1.join();
}