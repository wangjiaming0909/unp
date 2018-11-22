#ifndef _UNP_SELECT_REACTOR_IMPL_H_
#define _UNP_SELECT_REACTOR_IMPL_H_

#include "reactor_implementation.h"
#include <vector>
#include "../util/easylogging++.h"
#include "../net/macros.h"

namespace reactor{

//event tuple
struct select_event_tuple{
    using Event_Type = event_handler::Event_Type;
    select_event_tuple() 
      : event_handler_(0), 
        event_type_(0)
    {}
    //event_handler
    event_handler  *event_handler_;
    //event_type
    Event_Type event_type_;
    //handle
    // int handle_;
};

class select_demultiplex_table{
public:
    using Event_Type = event_handler::Event_Type;
    select_demultiplex_table(size_t capacity) : table_(){
        if(capacity > MAX_NUMBER_OF_HANDLE) {
            LOG(ERROR) << "select_demultiplex_table capacity specified is capacity, bigger than " << MAX_NUMBER_OF_HANDLE;
            table_.reserve(MAX_NUMBER_OF_HANDLE);
        } else table_.reserve(capacity);
    }
    const std::vector<select_event_tuple>& get_event_tuple_array() const { return table_; }
    int bind(int handle, event_handler* handler, Event_Type type){
        if(handle == INVALID_HANDLE || handler == 0 || handle > MAX_NUMBER_OF_HANDLE) return -1;
        if(table_.capacity() < handle)
            table_.reserve(handle + handle/2);
        table_[handle].event_handler_ = handler;
        table_[handle].event_type_ = type;
        return 0;
    }
    int unbind(int handle){

    }

private:
    std::vector<select_event_tuple> table_;
public:
    static const long int MAX_NUMBER_OF_HANDLE = FD_SETSIZE;
};
const long int select_demultiplex_table::MAX_NUMBER_OF_HANDLE;

class select_reactor_impl : public reactor_implementation{
public:
    void handle_events(std::chrono::microseconds *timeout) override{ }
    void register_handler(event_handler* handler, Event_Type type) override{

    }
    void remove_handler(event_handler *handler, Event_Type type) override{ }
    // void register_handler(int handle, event_handler *handler, Event_Type type) override{ }
    // void remove_handler(int handle, event_handler *handler, Event_Type type) override{ }
private:
    select_demultiplex_table demux_table_;
};


}

#endif //_UNP_SELECT_REACTOR_IMPL_H_
