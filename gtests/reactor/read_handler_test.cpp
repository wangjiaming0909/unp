#include <gtest/gtest.h>
#include "server/reactor/read_handler.h"
#include "server/reactor/select_reactor_impl.h"
#include "server/reactor/acceptor.h"


using namespace reactor;
using namespace thread;
using namespace net;

TEST(read_handler_test, integration_with_acceptor){
    // reactor::Reactor rt{new reactor::select_reactor_impl{}};
    // inet_addr listen_addr{9000, "127.0.0.1"};
    // reactor_acceptor acceptor{rt, listen_addr};
    // rt.handle_events();
    // std::this_thread::sleep_for(std::chrono::seconds(1000));
}
