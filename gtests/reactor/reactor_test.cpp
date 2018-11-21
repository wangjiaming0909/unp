#include <gtest/gtest.h>
#include "../../server/reactor/reactor.h"

TEST(test_reactor, instance_function_can_get_a_instance_of_reactor){
    auto react = reactor::reactor::instance();
    ASSERT_TRUE(react != 0);
    auto ptr = react.get();
    ASSERT_TRUE(ptr != 0);
}