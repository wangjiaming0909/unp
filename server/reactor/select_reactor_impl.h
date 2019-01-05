#ifndef _UNP_SELECT_REACTOR_IMPL_H_
#define _UNP_SELECT_REACTOR_IMPL_H_

#include "reactor_implementation.h"
#include <vector>
#include "server/util/easylogging++.h"
#include "server/net/macros.h"
#include "server/net/handle_set.h"
#include "event_handler.h"
#include <sys/select.h>
#include <unordered_map>

namespace reactor{

struct select_reactor_handle_sets{
    select_reactor_handle_sets() 
        : read_set()
        , write_set()
        , exception_set(){}
    unp::handle_set read_set;
    unp::handle_set write_set;
    unp::handle_set exception_set;
};

// struct handler_and_type{
//     using Event_Type = event_handler::Event_Type;
//     handler_and_type(event_handler* handler, Event_Type type)
//         : event_handler_(handler), event_type_(type){}
//     bool operator==(const handler_and_type& other){
//         return (event_handler_ == other.event_handler_) &&
//                (event_type_ == other.event_type_);
//     }
    
//     event_handler*      event_handler_;
//     Event_Type          event_type_;
// };

//event tuple
struct select_event_tuple{
    using Event_Type = event_handler::Event_Type;
    using type_handler_pair = std::pair<Event_Type, event_handler*>;
    select_event_tuple (int handle = INVALID_HANDLE) : handle(handle), types_and_handlers() { 
        types_and_handlers.resize(64);
        for(int i = 0; i < 64; i++){
            types_and_handlers[i] = nullptr;
        }
    }
    // int bind_new(const handler_and_type& handler_type){
    //     return bind_new(handler_type.event_type_, handler_type.event_handler_);
    // }
    int bind_new(Event_Type type, event_handler* handler){
        if(handle == INVALID_HANDLE) return -1;
        if(type == event_handler::NONE || handler == 0){
            LOG(WARNING) << "can't bind NONE event or handler is nullptr";
            return -1;
        }
        types_and_handlers[type] = handler;
        
        return 0;
    }
    // int unbind(const handler_and_type& handler_type){
    //     if(handle == INVALID_HANDLE) return -1;
    //     if(handlers_types_.count(handler_type) != 1){
    //         return -1;
    //     }
    //     int ret = handlers_types_.erase(handler_type);
    //     if(ret <= 0) return -1;
    //     return 0;
    // }
    int unbind(Event_Type type, event_handler* handler){
        if(handle == INVALID_HANDLE) return -1;
        if(types_and_handlers[type] != nullptr) {
            LOG(WARNING) << "can't unbind, no this type of event";
            return -1;
        }
        if(types_and_handlers[type] != handler) {
            LOG(WARNING) << "the handler you unbinded is not the same as you provided";
        }
        types_and_handlers[type] = nullptr;
        return 0;
    }

    event_handler* get_handler(Event_Type type) const {
        if(types_and_handlers[type] == nullptr) {
            if(type == event_handler::READ_EVENT){
                if(types_and_handlers[event_handler::ACCEPT_EVENT] != nullptr){
                    return types_and_handlers[event_handler::ACCEPT_EVENT];
                }
            }
            LOG(WARNING) 
                << "handle " << handle 
                << " has no handler for type " 
                << event_type_to_string(type);
            return nullptr;
        }
        return types_and_handlers[type];
    }

    void clear() {types_and_handlers.clear(); }

    int                                                     handle;
    //for handler and type
    //one handle:
    //one type of event, only have one handler, 如果可以有两个，那么事件发生的时候我调用谁呢?
    //but one handler, 可以有多个事件与之对应
    // std::unordered_map<Event_Type, event_handler*>          types_and_handlers;
    std::vector<event_handler*>                             types_and_handlers;
};

class select_demultiplex_table{
public:
    using Event_Type = event_handler::Event_Type;
    select_demultiplex_table(size_t capacity = 8);
    event_handler* get_handler(int handle, Event_Type type) const;
    const std::vector<select_event_tuple>& get_event_tuple_array() const { return table_; }
    int bind(int handle, event_handler* handler, Event_Type type);
    // int bind(const select_event_tuple& event_tuple);
    //unbind掉绑定到这个handle的所有事件处理器
    int unbind(int handle);
    int unbind(int handle, const event_handler* handler);
    // int unbind(const select_event_tuple& event_tuple);
    int get_current_max_handle_p_1() const { return current_max_handle_p_1_;}
private:
    bool is_handle_in_range(int handle) const {
        if(handle < 0 || ((handle + 1) > current_max_handle_p_1_)) 
            return false;
        if(table_[handle].types_and_handlers.size() == 0) {
            // LOG(WARNING) << "handle is in the table, but no handler and type registered...";
            return false;
        }
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

//?how to test it when using a system call select?
class select_reactor_impl : public reactor_implementation{
public:
    select_reactor_impl() 
        : reactor_implementation()
        , dispatch_sets_()
        , wait_sets_()
        , ready_sets_()
        , demux_table_(){}
    void handle_events(std::chrono::microseconds *timeout) override;
    void register_handler(event_handler* handler, Event_Type type) override;
    void unregister_handler(event_handler *handler, Event_Type type) override;
    void register_handler(int handle, event_handler *handler, Event_Type type) override;
    void unregister_handler(int handle, event_handler *handler, Event_Type type) override;
private:
    int select(std::chrono::microseconds* timeout);
    int dispatch(int active_handle_count);

//for io_dispatching
    int dispatch_io_handlers(int active_handle_count, int& io_handles_dispatched);
    int dispatch_io_set(
        int number_of_active_handles, 
        int& number_of_handles_dispatched,
        Event_Type type,
        unp::handle_set& dispatch_set,
        unp::handle_set& ready_set,
        HANDLER callback);
private:
    //track handles that are currently ready for dispatch
    select_reactor_handle_sets dispatch_sets_;
    //track handles that are waited for by <select>
    select_reactor_handle_sets wait_sets_;
    //track handles that are currently suspended
    // select_reactor_handle_sets suspend_set_;
    //track handles we are interested in for various events that must be dispatched
    //without going through <select>
    select_reactor_handle_sets ready_sets_;
    select_demultiplex_table demux_table_;
    // event_handler 
};


}

#endif //_UNP_SELECT_REACTOR_IMPL_H_
