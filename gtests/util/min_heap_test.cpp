#include <gtest/gtest.h>
#include "server/util/min_heap.h"

TEST(min_heap_test, min_heap_test_){
    util::min_heap<int> mh{};
    mh.push(12);
    mh.push(13);
}
