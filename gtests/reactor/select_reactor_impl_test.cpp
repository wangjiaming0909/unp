#include "gtest/gtest.h"
#include "../../server/reactor/select_reactor_impl.h"

TEST(select_reactor_impl_test, test_max_number_of_handles){
    long int number_of_handles = reactor::select_demultiplex_table::MAX_NUMBER_OF_HANDLE;
    ASSERT_TRUE(number_of_handles == 1024);
}

TEST(select_reactor_impl_test, test_set_table_size_1024){
    reactor::select_demultiplex_table table{1024};
    ASSERT_EQ(table.get_event_tuple_array().capacity(), 1024);
}
TEST(select_reactor_impl_test, test_invalid_capacity){
    reactor::select_demultiplex_table table2{1025};
    ASSERT_EQ(table2.get_event_tuple_array().capacity(), 1024);
}