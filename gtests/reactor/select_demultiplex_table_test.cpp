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
    select_demultiplex_table_test() : table_(){ 
        reactor_ = new MockReactor();
        handler1_ = new MockEventHandler(*reactor_);
        handler2_ = new MockEventHandler(*reactor_);
    }
    ~select_demultiplex_table_test() override{
        delete reactor_;
        delete handler1_;
        delete handler2_;
    }
    void SetUp() override{ }
    void TearDown() override{}
protected:
    select_demultiplex_table    table_;
    Reactor	                    *reactor_;
    MockEventHandler            *handler1_;
    MockEventHandler	        *handler2_;
};

TEST_F(select_demultiplex_table_test, test_bind){
    int handle1 = 1, handle2 = 2;
    unsigned int event_type1 = event_handler::READ_EVENT;
    table_.bind(handle1, this->handler1_, event_type1);
    ASSERT_EQ(table_.get_current_max_handle_p_1(), 2);

    auto event_vector = table_.get_event_vector();
    ASSERT_EQ(8, event_vector.size());//size
    ASSERT_EQ(table_.get_handler(handle1, event_type1), this->handler1_);//handler ptr
    //select_event_tuple shoule be right
    ASSERT_EQ(event_vector[handle1].types_and_handlers.size(), 64);
    ASSERT_EQ(event_vector[handle1].types_and_handlers[event_type1], this->handler1_);
    ASSERT_EQ(event_vector[handle1].event_count, 1);
    //other handles and other types should be right
    for(int handle = 0; handle < event_vector.size(); handle++){
        if(handle == 1) continue;
        ASSERT_EQ(event_vector[handle].types_and_handlers.size(), 64);
        ASSERT_EQ(event_vector[handle].event_count, 0);
        ASSERT_EQ(event_vector[handle].types_and_handlers[event_type1], nullptr);
    }

    //bind the second handle
    unsigned int event_type2 = event_handler::WRITE_EVENT;
    table_.bind(handle2, this->handler2_, event_type2);

    //table_ property
    ASSERT_EQ(table_.get_current_max_handle_p_1(), 3);
    auto event_vector2 = table_.get_event_vector();
    ASSERT_EQ(8, event_vector2.size());
    //--------------for handle1 parts
    ASSERT_EQ(table_.get_handler(handle1, event_type1), this->handler1_);//handler ptr
    //select_event_tuple shoule be right
    ASSERT_EQ(event_vector2[handle1].types_and_handlers.size(), 64);
    ASSERT_EQ(event_vector2[handle1].types_and_handlers[event_type1], this->handler1_);
    ASSERT_EQ(event_vector2[handle1].event_count, 1);
    //--------------for handle1 parts
    for(int handle = 0; handle < event_vector2.size(); handle++){
        if(handle == 1 || handle == 2) continue;
        ASSERT_EQ(event_vector[handle].types_and_handlers.size(), 64);
        ASSERT_EQ(event_vector[handle].event_count, 0);
        ASSERT_EQ(event_vector[handle].types_and_handlers[event_type1], nullptr);
    }


}