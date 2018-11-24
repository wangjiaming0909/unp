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
    select_event_tuple(
        event_handler* handler = 0, 
        int handle = INVALID_HANDLE, 
        Event_Type type = 0) 
      : event_handler_(handler), 
        handle_(handle),
        event_type_(type)
    {}
    //event_handler
    event_handler  *event_handler_;
    //handle
    int handle_;
    //event_type
    Event_Type event_type_;
};

class select_demultiplex_table{
public:
    using Event_Type = event_handler::Event_Type;
    select_demultiplex_table(size_t capacity) : table_(){
        // LOG(TRACE) << "select_demultiplex_table";
        if(capacity > MAX_NUMBER_OF_HANDLE) {
            LOG(ERROR) << "select_demultiplex_table capacity specified is capacity, bigger than " << MAX_NUMBER_OF_HANDLE;
            table_.reserve(MAX_NUMBER_OF_HANDLE);
        } else table_.reserve(capacity);
    }
    event_handler* get_handler(int handle) const{
        if(!is_handle_in_range(handle)){
            LOG(ERROR) << "handle is not in range, handle: " << handle;
            return 0;
        }
        return table_[handle].event_handler_;
    }
    const std::vector<select_event_tuple>& get_event_tuple_array() const { return table_; }
    int bind(int handle, event_handler* handler, Event_Type type){
        // if(handle == INVALID_HANDLE || handler == 0 || handle > MAX_NUMBER_OF_HANDLE) return -1;
        if(!is_valid_handle(handle) || handler == 0) {
            LOG(ERROR) << "handle is not in range or handler is null handle: " 
                       << handle << " handler: " << handler;
            return -1;
        }
        if(table_.capacity() < handle)
            table_.reserve(handle + handle/2);
        table_[handle].event_handler_ = handler;
        table_[handle].event_type_ = type;
        if(handle > current_max_handle_p_1_ - 1){
            current_max_handle_p_1_ = handle + 1;
            table_.resize(current_max_handle_p_1_);
        }
        return 0;
    }
    int bind(const select_event_tuple& event_tuple){
        return bind(
            event_tuple.handle_, 
            event_tuple.event_handler_, 
            event_tuple.event_type_);
    }
    int unbind(int handle){
        if(!is_handle_in_range(handle) || table_[handle].event_handler_ == 0){
            LOG(WARNING) << "handle is not in the table, handle: " << handle;
            return -1;
        }
        //if handle is the max_handle - 1
        int tmp = handle;
        if(current_max_handle_p_1_ == handle + 1){
            while(table_[--tmp].handle_ == INVALID_HANDLE){
                
            }
        }
        table_[handle].event_handler_ = 0;
        table_[handle].event_type_ = 0;

        return 0;
    }
    int get_current_max_handle_p_1() const { return current_max_handle_p_1_;}
private:
    bool is_handle_in_range(int handle) const {
        if(handle < 0 || handle > table_.size()) return false;
        return true;
    }
    bool is_valid_handle(int handle) const {
        if(handle == INVALID_HANDLE || handle > MAX_NUMBER_OF_HANDLE)
            return false;
        return true;
    }


private:
    std::vector<select_event_tuple> table_;
    int current_max_handle_p_1_ = INVALID_HANDLE;
public:
    static const long int MAX_NUMBER_OF_HANDLE = FD_SETSIZE;
};
const long int select_demultiplex_table::MAX_NUMBER_OF_HANDLE;

class select_reactor_impl : public reactor_implementation{
public:
    void handle_events(std::chrono::microseconds *timeout) override{ 

    }
    void register_handler(event_handler* handler, Event_Type type) override{

    }
    void remove_handler(event_handler *handler, Event_Type type) override{ }
    // void register_handler(int handle, event_handler *handler, Event_Type type) override{ }
    // void remove_handler(int handle, event_handler *handler, Event_Type type) override{ }
private:
    int select(select_reactor_handle_set& dispatch_set, std::chrono::microseconds* timeout){
        // ::select()
    }
    int dispatch(){

    }
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
