#ifndef _UNP_SELECT_REACTOR_IMPL_H_
#define _UNP_SELECT_REACTOR_IMPL_H_

#include "reactor_implementation.h"
#include <vector>
#include "../util/easylogging++.h"
#include "../net/macros.h"
#include "../net/handle_set.h"
#include "event_handler.h"
#include <sys/select.h>

namespace reactor{

struct select_reactor_handle_set{
    unp::handle_set read_set;
    unp::handle_set write_set;
    unp::handle_set exception_set;
};

//event tuple
struct select_event_tuple{
    using Event_Type = event_handler::Event_Type;
    select_event_tuple( event_handler* handler = 0, 
                        int handle = INVALID_HANDLE, 
                        Event_Type type = 0) 
      : event_handler_(handler), 
        handle_(handle),
        event_type_(type) {}
    event_handler  *event_handler_;
    int handle_;
    Event_Type event_type_;
};

class select_demultiplex_table{
public:
    using Event_Type = event_handler::Event_Type;
    select_demultiplex_table(size_t capacity);
    event_handler* get_handler(int handle) const;
    const std::vector<select_event_tuple>& get_event_tuple_array() const { return table_; }
    int bind(int handle, event_handler* handler, Event_Type type);
    int bind(const select_event_tuple& event_tuple);
    int unbind(int handle);
    int get_current_max_handle_p_1() const { return current_max_handle_p_1_;}
private:
    bool is_handle_in_range(int handle) const {
        if(handle < 0 || ((handle + 1) > current_max_handle_p_1_)) 
            return false;
        if(table_[handle].event_handler_ == 0) 
            return false;
        return true;
    }
    bool is_valid_handle(int handle) const {
        if(handle == INVALID_HANDLE || handle > MAX_NUMBER_OF_HANDLE)
            return false;
        return true;
    }

private:
    //the size of table_ is meaningless
    std::vector<select_event_tuple> table_;
    int current_max_handle_p_1_ = INVALID_HANDLE + 1;
public:
    static const long int MAX_NUMBER_OF_HANDLE = FD_SETSIZE;
};

typedef int (event_handler::*HANDLER)(int);

class select_reactor_impl : public reactor_implementation{
public:
    void handle_events(std::chrono::microseconds *timeout) override;
    void register_handler(event_handler* handler, Event_Type type) override;
    void remove_handler(event_handler *handler, Event_Type type) override;
    // void register_handler(int handle, event_handler *handler, Event_Type type) override{ }
    // void remove_handler(int handle, event_handler *handler, Event_Type type) override{ }
private:
    int select(std::chrono::microseconds* timeout);
    int dispatch();

//for io_dispatching
    int dispatch_io_handlers(int active_handle_count, int& io_handles_dispatched);
    int dispatch_io_set(
        int number_of_active_handlers, 
        int& number_of_handlers_dispatched,
        Event_Type type,
        HANDLER callback);
private:
    //track handles that are currently ready for dispatch
    select_reactor_handle_set dispatch_set_;
    //track handles that are waited for by <select>
    select_reactor_handle_set wait_set_;
    //track handles that are currently suspended
    // select_reactor_handle_set suspend_set_;
    //track handles we are interested in for various events that must be dispatched
    //without going through <select>
    select_reactor_handle_set ready_set;
    select_demultiplex_table demux_table_;
    // event_handler 
};


}

#endif //_UNP_SELECT_REACTOR_IMPL_H_
