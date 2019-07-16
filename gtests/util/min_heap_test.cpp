#include <gtest/gtest.h>
#include "util/min_heap.h"
#include "reactor/TimeoutHandler.h"

TEST(min_heap_test, min_heap_test_){
    util::min_heap<int> mh{};
    mh.push(12);
    mh.push(13);

    auto top = mh.top();
    ASSERT_EQ(top, 12);
}

TEST(min_heap_test, using_TimeoutHandelr_pointer)
{
    using namespace util;
    using namespace reactor;
    using namespace std::chrono_literals;

    min_heap<TimeoutHandler*, TimeoutHandlerComparer<TimeoutHandler*>> handlers;
    auto handler1 = new TimeoutHandler();
    auto handler2 = new TimeoutHandler();
    auto handler3 = new TimeoutHandler();

    handler1->expiration_ = std::chrono::steady_clock::now();
    handler2->expiration_ = handler1->expiration_ + 2s;
    handler3->expiration_ = handler2->expiration_ + 2s;

    handlers.push(handler1);
    handlers.push(handler2);
    handlers.push(handler3);

    auto top = handlers.top();
    ASSERT_EQ(top, handler1);
    handlers.pop();
    top = handlers.top();
    ASSERT_EQ(top, handler2);
    handlers.pop();
    top = handlers.top();
    ASSERT_EQ(top, handler3);
}


