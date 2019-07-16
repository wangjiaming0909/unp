#include "gtest/gtest.h"
#include "reactor/select_reactor_impl.h"
#include "reactor/EventHandler.h"
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
//     default_EventHandler handler{react};
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
//     default_EventHandler handler{react};
//     default_EventHandler handler2{react};
//     default_EventHandler handler3{react};
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
    // default_EventHandler handler1{react};
    // react_select.register_handler(handle1, &handler1, EventHandler::READ_EVENT);
    // react_select.handle_events(0);
    ASSERT_EQ(1, 1);
}

TEST(select_demultiplex_table, bind_unbind_timeoutHandlers)
{
    using namespace reactor;
    using namespace std::chrono_literals;
    select_demultiplex_table table{4};
    TimeoutHandler* handler1 = new TimeoutHandler();
    handler1->expiration_ = std::chrono::steady_clock::now();
    TimeoutHandler *handler2 = new TimeoutHandler();
    handler2->expiration_ = handler1->expiration_ + 2s;
    TimeoutHandler* handler3 = new TimeoutHandler();
    handler3->expiration_ = handler2->expiration_ + 2s;

    LOG(INFO) << "handler1: " << handler1->expiration_.time_since_epoch().count();
    LOG(INFO) << "handler2: " << handler2->expiration_.time_since_epoch().count();
    LOG(INFO) << "handler3: " << handler3->expiration_.time_since_epoch().count();

    LOG(INFO) << "address: handler1: " << handler1;
    LOG(INFO) << "address: handler2: " << handler2;
    LOG(INFO) << "address: handler3: " << handler3;

    table.bindTimeoutEvent(*handler1);
    table.bindTimeoutEvent(*handler2);
    table.bindTimeoutEvent(*handler3);

    auto handler = table.getLatestTimeoutHandler();
    ASSERT_EQ(handler, handler1);
    table.unbindTimeoutEvent(*handler1);
    handler = table.getLatestTimeoutHandler();
    ASSERT_EQ(handler, handler2);
    table.unbindTimeoutEvent(*handler2);
    handler = table.getLatestTimeoutHandler();
    ASSERT_EQ(handler, handler3);
    bool hasTimeoutEvent = table.hasEvent(EventHandler::TIMEOUT_EVENT);
    ASSERT_EQ(hasTimeoutEvent, true);
    table.unbindTimeoutEvent(*handler3);
    handler = table.getLatestTimeoutHandler();
    ASSERT_EQ(handler, nullptr);
    hasTimeoutEvent = table.hasEvent(EventHandler::TIMEOUT_EVENT);
    ASSERT_EQ(hasTimeoutEvent, false);
    ASSERT_EQ(table.timeoutHandlersMinHeap_.empty(), true);
}
