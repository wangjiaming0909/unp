#include <gtest/gtest.h>
#include "server/thread/task.h"
#include "server/reactor/select_reactor_impl.h"
#include "server/reactor/reactor.h"
#include "server/util/easylogging++.h"
#include <memory>
#include <chrono>
#include <thread>
using namespace std::chrono_literals;

using namespace thread;
using namespace reactor;

class fake_task : public task<int>{
public:
    fake_task(reactor::Reactor* react, thread_pool* pool, message_queue<int>* mq) : task<int>(react, pool, mq){} 
    virtual int routine() override {
        micro_seconds _{0};
        auto data_ptr = this->get_data(_);
        data_ptr->operator*()++;
        // LOG(INFO) << data_ptr->operator*()++ << " " << std::this_thread::get_id();
    }
};

TEST(task_test, test_task_constructor){
//    thread_pool pool{1};
//    reactor::Reactor rt{new select_reactor_impl()};
//    pool.start();
//    message_queue<int> mq{};
//    fake_task t{&rt, &pool, &mq};
//    int a = 10;
//    data_block<int> data(&a, false);
//    auto *p = &data;
//    t.put_data(p, 0);
//    t.activate(1);
//    std::chrono::microseconds timeout{};
//    timeout = 3s;
//    t.wait(&timeout);
//    ASSERT_EQ(a, 11);
}

TEST(task_test, test_one_task_using_two_threads){
    thread_pool pool{4};
    reactor::Reactor rt{new select_reactor_impl()};
    pool.start();
    message_queue<int> mq{};
    fake_task t{&rt, &pool, &mq};
    int a = 0;
    data_block<int> data(&a, false);
    auto *p = &data;
    std::chrono::seconds _{0};
    t.put_data(p, _);
    t.activate(1);
    std::chrono::microseconds timeout{};
    timeout = 3s;
    t.wait(&timeout);
    ASSERT_EQ(a, 1);
    LOG(INFO) << "OVER...";
}
