#include <gtest/gtest.h>
#include "server/reactor/read_handler.h"
#include "server/reactor/select_reactor_impl.h"

using namespace reactor;
using namespace thread;
using namespace std::chrono_literals;

TEST(io_handler_test, constructor_test){
    // Reactor reactor{new select_reactor_impl{}};
    // thread_pool pool{2};
    // pool.start();
    // message_queue<int> queue{};
    // ReadHandler<int> handler{reactor, queue, pool};
    // int a = 0;
    // data_block<int> data(&a, false);
    // handler.open();
    // handler.put_data_and_activate(data, 0s);
}