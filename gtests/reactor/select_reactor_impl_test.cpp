#include "gtest/gtest.h"
#include "../../server/reactor/select_reactor_impl.h"

TEST(select_reactor_impl_test, test_max_number_of_handles){
    long int number_of_handles = reactor::select_demultiplex_table::MAX_NUMBER_OF_HANDLE;
    ASSERT_TRUE(number_of_handles == 1024);
}

TEST(select_reactor_impl_test, test_set_table_size_1024){
    reactor::select_demultiplex_table table{1024};
    ASSERT_EQ(table.get_event_tuple_array().size(), 1024);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 0);
}

TEST(select_reactor_impl_test, test_invalid_capacity){
    reactor::select_demultiplex_table table{1025};
    ASSERT_EQ(table.get_event_tuple_array().size(), 1024);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 0);
}

TEST(select_reactor_impl_test, test_bind_event_tuple){
    using namespace reactor;
    int handle1 = 1;
    default_event_handler handler{handle1};
    select_event_tuple event_tuple{&handler, handle1, 01};

    select_demultiplex_table table{4};
    table.bind(event_tuple);
    ASSERT_EQ(table.get_event_tuple_array().size(), 4);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 2);
}

TEST(select_reactor_impl_test, test_bind_3_event_tuple_table_size_and_unbind){
    using namespace reactor;
    int handle1 = 1, handle2 = 3, handle3 = 4;;
    default_event_handler handler{handle1};
    default_event_handler handler2{handle2};
    default_event_handler handler3{handle3};
    select_event_tuple event_tuple{&handler, handle1, 01};
    select_event_tuple event_tuple2{&handler2, handle2, 01};
    select_event_tuple event_tuple3{&handler3, handle3, 01};

    select_demultiplex_table table{4};
    table.bind(event_tuple);
    table.bind(event_tuple2);
    ASSERT_EQ(table.get_event_tuple_array().size(), 4);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 4);
    table.unbind(handle1);
    ASSERT_EQ(table.get_event_tuple_array().size(), 4);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 4);
    table.unbind(handle2);
    table.unbind(handle3);//no effect
    ASSERT_EQ(table.get_event_tuple_array().size(), 4);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 0);
    table.bind(event_tuple3);//will resize the table
    ASSERT_TRUE(int(table.get_event_tuple_array().size()) > event_tuple3.handle_);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 5);
    table.unbind(handle3);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 0);
}
