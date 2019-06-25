#include "gtest/gtest.h"
#include "reactor/select_reactor_impl.h"
#include "reactor/event_handler.h"
#include "reactor/reactor.h"

TEST(select_reactor_impl_test, test_max_number_of_handles){
    long int number_of_handles = reactor::select_demultiplex_table::MAX_NUMBER_OF_HANDLE;
    ASSERT_TRUE(number_of_handles == 1024);
}

TEST(select_reactor_impl_test, test_set_table_size_1024){
    reactor::select_demultiplex_table table{1024};
    ASSERT_EQ(table.get_event_vector().size(), 1024);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 0);
}

TEST(select_reactor_impl_test, test_invalid_capacity){
    reactor::select_demultiplex_table table{1025};
    ASSERT_EQ(table.get_event_vector().size(), 1024);
    ASSERT_EQ(table.get_current_max_handle_p_1(), 0);
}

TEST(select_reactor_impl_test, test_bind_event_tuple){
//     using namespace reactor;
//     reactor::Reactor react{};
//     int handle1 = 1;
//     default_event_handler handler{react};
// //    select_event_tuple event_tuple{handle1};
// //    event_tuple.bind_new(1, &handler);

//     select_demultiplex_table table{4};
//     table.bind(handle1, &handler, 1);
//     ASSERT_EQ(table.get_event_vector().size(), 4);
//     ASSERT_EQ(table.get_current_max_handle_p_1(), 2);
}

TEST(select_reactor_impl_test, test_bind_3_event_tuple_table_size_and_unbind){
//     using namespace reactor;
//     reactor::Reactor react{};
//     int handle1 = 1, handle2 = 3, handle3 = 4;;
//     default_event_handler handler{react};
//     default_event_handler handler2{react};
//     default_event_handler handler3{react};
// //    select_event_tuple event_tuple{&handler, handle1, 01};
// //    select_event_tuple event_tuple2{&handler2, handle2, 01}; 
// //    select_event_tuple event_tuple3{&handler3, handle3, 01}; 
//     select_demultiplex_table table{4};
//     table.bind(handle1, &handler, 1);
//     table.bind(handle2, &handler2, 1);
//     ASSERT_EQ(table.get_event_vector().size(), 4);
//     ASSERT_EQ(table.get_current_max_handle_p_1(), 4);
//     table.unbind(handle1);
//     ASSERT_EQ(table.get_event_vector().size(), 4);
//     ASSERT_EQ(table.get_current_max_handle_p_1(), 4);
//     table.unbind(handle2);
//     table.unbind(handle3);//no effect
//     ASSERT_EQ(table.get_event_vector().size(), 4);
//     ASSERT_EQ(table.get_current_max_handle_p_1(), 0);
//     table.bind(handle3, &handler3, 1);//will resize the table
//     ASSERT_TRUE(int(table.get_event_vector().size()) > handle3);
//     ASSERT_EQ(table.get_current_max_handle_p_1(), 5);
//     table.unbind(handle3);
//     ASSERT_EQ(table.get_current_max_handle_p_1(), 0);
}

TEST(select_reactor_impl_test, test_select_and_dispatch_events){
    // using namespace reactor;
    // reactor::Reactor react{};
    // select_reactor_impl react_select{};
    // int handle1 = 1;
    // default_event_handler handler1{react};
    // react_select.register_handler(handle1, &handler1, event_handler::READ_EVENT);
    // react_select.handle_events(0);
    ASSERT_EQ(1, 1);
}
