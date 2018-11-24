#include "gtest/gtest.h"
#include "../../server/reactor/select_reactor_impl.h"

TEST(select_reactor_impl_test, test_max_number_of_handles){
    long int number_of_handles = reactor::select_demultiplex_table::MAX_NUMBER_OF_HANDLE;
    ASSERT_TRUE(number_of_handles == 1024);
}

TEST(select_reactor_impl_test, test_set_table_size_1024){
    reactor::select_demultiplex_table table{1024};
    ASSERT_EQ(table.get_event_tuple_array().capacity(), 1024);
    ASSERT_EQ(table.get_event_tuple_array().size(), 0);
}

TEST(select_reactor_impl_test, test_invalid_capacity){
    reactor::select_demultiplex_table table{1025};
    ASSERT_EQ(table.get_event_tuple_array().capacity(), 1024);
}

TEST(select_reactor_impl_test, test_bind_event_tuple){
    using namespace reactor;
    int handle1 = 1;
    default_event_handler handler{handle1};
    select_event_tuple event_tuple{&handler, handle1, 01};

    select_demultiplex_table table{4};
    table.bind(event_tuple);
    ASSERT_EQ(table.get_event_tuple_array().capacity(), 4);
    ASSERT_EQ(table.get_event_tuple_array().size(), 2);
}

TEST(select_reactor_impl_test, test_bind_2_event_tuple_table_size){
    using namespace reactor;
    int handle1 = 1, handle2 = 3;
    default_event_handler handler{handle1};
    default_event_handler handler2{handle2};
    select_event_tuple event_tuple{&handler, handle1, 01};
    select_event_tuple event_tuple2{&handler2, handle2, 01};

    select_demultiplex_table table{4};
    table.bind(event_tuple);
    table.bind(event_tuple2);
    ASSERT_EQ(table.get_event_tuple_array().capacity(), 4);
    ASSERT_EQ(table.get_event_tuple_array().size(), 4);
}