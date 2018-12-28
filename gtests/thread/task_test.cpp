#include <gtest/gtest.h>
#include "server/thread/task.h"
#include "server/reactor/select_reactor_impl.h"
#include "server/reactor/reactor.h"
#include <memory>
#include <chrono>
using namespace std::chrono_literals;

using namespace thread;
using namespace reactor;

class fake_task : public task<int>{
public:
    fake_task(reactor::Reactor* react, thread_pool* pool, message_queue<int>* mq) : task<int>(react, pool, mq){}
   virtual int routine() override {
       auto *p_data = new data_block<int>();
       this->get_data(p_data, 0);
       p_data->operator*() = 1;
       delete p_data;
   }
};

TEST(task_test, test_task_constructor){
    thread_pool pool{1};
    reactor::Reactor rt{new select_reactor_impl()};
    pool.start();
    message_queue<int> mq{};
    fake_task t{&rt, &pool, &mq};
    int a = 10;
    data_block<int> data(&a, false);
    auto *p = &data;
    t.put_data(p, 0);
    t.activate(1);
    std::chrono::microseconds timeout{};
    timeout = 5s;
    t.wait(&timeout);
    ASSERT_EQ(a, 1);
}