#include <gtest/gtest.h>
#include "../../server/reactor/reactor.h"
#include "server/reactor/select_reactor_impl.h"

using namespace reactor;

TEST(test_reactor, constructor_of_reactor){
    Reactor react{new select_reactor_impl{}};
    ASSERT_EQ(1, 1);
}