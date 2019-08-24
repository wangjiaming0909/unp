#include <gtest/gtest.h>
#include "reactor/tcp_server.h"
#include "reactor/echo_connection_handler.h"

TEST(TcpServer, normal)
{
    // GTEST_SKIP();
    using namespace reactor;
    using namespace std::chrono_literals;

    net::inet_addr listenAddr{9090, "192.168.0.111"};
    tcp_server<echo_connection_handler> server{listenAddr};

    server.set_thread_num(4);

    std::thread th1{&tcp_server<echo_connection_handler>::start, &server, unp::reactor_imp_t_enum::USING_EPOLL};
    // server.suspend();

    // server.stop();
    th1.join();
}