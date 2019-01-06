#include <gtest/gtest.h>
#include "server/reactor/select_reactor_impl.h"
#include "server/reactor/event_handler.h"
#include "server/reactor/select_reactor_impl.h"
#include "server/reactor/reactor.h"
#include <gmock/gmock.h>

using namespace reactor;

class MockReactor : public Reactor {
public:
    MockReactor() : Reactor() {}
};

class MockEventHandler : public event_handler {
public:
    MockEventHandler(Reactor& react) : event_handler(react) {}
};

class select_demultiplex_table_test : public ::testing::Test{
protected:
    select_demultiplex_table_test()
    : table_() , reactor_()
    , handler1_(reactor_)
    , handler2_(reactor_){ }
    ~select_demultiplex_table_test() override{}
    void SetUp() override{ }
    void TearDown() override{}
protected:
    select_demultiplex_table    table_;
    MockReactor                 reactor_;
    MockEventHandler	        handler1_;
    MockEventHandler	        handler2_;

};

TEST_F(select_demultiplex_table_test, test_bind){
    int handle1 = 1, handle2 = 2;
    table_.bind(handle1, &this->handler1_, event_handler::READ_EVENT);
    ASSERT_EQ(table_.get_current_max_handle_p_1(), 2);

    auto event_tuple = table_.get_event_vector();
}