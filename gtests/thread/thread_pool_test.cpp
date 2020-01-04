#include <gtest/gtest.h>
#include "thread/thread_pool.h"
#include <iostream>

using namespace thread;

TEST(thread_pool_test, test_cancel){
    thread_pool pool(2);
    pool.start();

    int a = 10;
    int *pa = &a;
    pool.add_task([=]() {
        std::cout << *pa << std::endl;
    });
    pool.add_task([=]() {
        std::cout << *pa << std::endl;
    });

    pool.cancel();
    ASSERT_EQ(1,1);
}