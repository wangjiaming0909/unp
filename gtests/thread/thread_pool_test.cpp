#include <gtest/gtest.h>
#include "server/thread/thread_pool.h"

using namespace thread;

TEST(thread_pool_test, test_cancel){
    thread_pool pool(2);

    pool.start();
    pool.cancel();
    ASSERT_EQ(1,1);
}