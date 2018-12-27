#include <gtest/gtest.h>
#include "server/thread/task.h"

using namespace thread;

class fake_task : public task<int>{
public:
    fake_task(thread_pool* pool, message_queue<int>* mq) : task<int>(pool, mq){}
   virtual int routine() override {

   }
};

TEST(task_test, test_task_constructor){
    thread_pool pool{2};
    pool.start();
    message_queue<int> mq{};
    fake_task t{&pool, &mq};
}