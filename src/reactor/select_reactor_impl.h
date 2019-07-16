#ifndef _UNP_SELECT_REACTOR_IMPL_H_
#define _UNP_SELECT_REACTOR_IMPL_H_

#include "reactor_implementation.h"
#include "util/easylogging++.h"
#include "net/macros.h"
#include "net/handle_set.h"
#include "EventHandler.h"
#include "util/min_heap.h"
#include "reactor/TimeoutHandler.h"

#include <sys/select.h>
#include <unordered_map>
#include <vector>

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

//event tuple
struct select_event_tuple{
    using Event_Type = EventHandler::Event_Type;
    select_event_tuple ()
    // : types_and_handlers()
    : types_and_handlers_()
    // , event_count(0)
    {
        // types_and_handlers.resize(64);
        // for(int i = 0; i < 64; i++){
        //     types_and_handlers[i] = nullptr;
        // }
    }

    int bind_new(Event_Type type, EventHandler* handler){
        if(type == EventHandler::NONE || handler == nullptr){
            LOG(WARNING) << "can't bind NONE event or handler is nullptr";
            return -1;
        }
        // types_and_handlers[type] = handler;
        types_and_handlers_.push_back(std::make_pair(type, handler));
        // event_count++;
        return 0;
    }

    int unbind(Event_Type type, const EventHandler* handler){
        if(type == EventHandler::NONE || handler == 0)
        {
            LOG(WARNING) << "can't unbind NONE event or handler can't be nullptr";
            return -1;
        }

        auto handler_find = std::find_if(types_and_handlers_.begin(), types_and_handlers_.end(),
            [&type](const std::pair<unsigned int, EventHandler*>& pair)
            {
                return pair.first == type;
            });

        if(handler_find == types_and_handlers_.end())
        {
            LOG(WARNING) << "can't unbind, no this type of event";
            return -1;
        }

        if(handler_find->second != handler)
        {
            LOG(WARNING) << "the handler you unbinded is not the same as you provided";
            return -1;
        }

        //unbind the handler
        types_and_handlers_.erase(handler_find);
        return 0;

        // if(types_and_handlers[type] == nullptr) {
        //     LOG(WARNING) << "can't unbind, no this type of event";
        //     return -1;
        // }
        // if(types_and_handlers[type] != handler) {
        //     LOG(WARNING) << "the handler you unbinded is not the same as you provided";
        //     return -1;
        // }
        // types_and_handlers[type] = nullptr;
        // event_count--;
        // return 0;
    }

    EventHandler* get_handler(Event_Type type) const {
        auto handler_find = std::find_if(types_and_handlers_.begin(), types_and_handlers_.end(),
            [&type](const std::pair<unsigned int, EventHandler*>& pair)
            {
                return pair.first == type;
            });

        if(handler_find == types_and_handlers_.end())
        {
            if(type == EventHandler::READ_EVENT)
            {
                return get_handler(EventHandler::ACCEPT_EVENT);
            }
        }

        // if(types_and_handlers[type] == nullptr) {
        //     if(type == EventHandler::READ_EVENT){//因为 acceptor 的处理事件也是读事件，以accept_event查找handler时，可能找不到
        //         return get_handler(EventHandler::ACCEPT_EVENT);
        //     }
        //     // LOG(WARNING)
        //     //     << "handle " << handle
        //     //     << " has no handler for type "
        //     //     << event_type_to_string(type);
        //     return nullptr;
        // }
        return handler_find->second;
    }

    void clear() {types_and_handlers_.clear(); }
    int handler_count() const {return types_and_handlers_.size();}

    //for handler and type
    //one handle:
    //one type of event, only have one handler, 如果可以有两个，那么事件发生的时候我调用谁呢?
    //but one handler, 可以有多个事件与之对应,其实也很少见,总不能一个读的handler又处理读事件又处理写事件吧
    //使用vector，表示一个handler可以绑定到不同的event上，但是一个event只能有一个handler
    // std::vector<EventHandler*>                                 types_and_handlers;
    std::vector<std::pair<unsigned int, EventHandler*>>        types_and_handlers_;
    // size_t                                                      event_count;
};

class select_demultiplex_table{
public:
    friend class select_reactor_impl;
    using Event_Type = EventHandler::Event_Type;
    select_demultiplex_table(size_t capacity = 8);
    EventHandler* get_handler(int handle, Event_Type type) const;
    const std::vector<select_event_tuple>& get_event_vector() const { return event_vector_; }
    int bind(int handle, EventHandler* handler, Event_Type type);
    //unbind掉绑定到这个handle的所有事件处理器
    int unbind(int handle);
    int unbind(int handle, const EventHandler* handler, Event_Type type);
    int get_current_max_handle_p_1() const { return current_max_handle_p_1_;}
    bool hasEvent(Event_Type type) const;

    //for timeout handlers
    TimeoutHandler *getTimeoutHandler() const;
    int bindTimeoutEvent(TimeoutHandler &handler);
    int unbindTimeoutEvent(TimeoutHandler &handler);
    TimeoutHandler* getLatestTimeoutHandler()
    {
        if(timeoutHandlersMinHeap_.empty()) return nullptr;
        return timeoutHandlersMinHeap_.top();
    }

private:
    bool hasEventForeachHandle(Event_Type type) const
    {
        for(int handle = 0; handle < current_max_handle_p_1_ - 1; handle++)
        {
            if(get_handler(handle, type)) return true;
        }
        return false;
    }
    bool is_handle_in_range(int handle) const ;
    bool is_valid_handle(int handle) const ;

private:
#ifdef TESTING
public:
#endif
    //the size of table_ is meaningless
    std::vector<select_event_tuple> event_vector_;
    int current_max_handle_p_1_ = INVALID_HANDLE + 1;
    util::min_heap<TimeoutHandler*, TimeoutHandlerComparer<TimeoutHandler*>> timeoutHandlersMinHeap_;

public:
    static const int MAX_NUMBER_OF_HANDLE = FD_SETSIZE;
};

typedef int (EventHandler::*HANDLER)(int);

//?how to test it when using a system call select?
//* select_reactor_impl 不是线程安全的
class select_reactor_impl : public reactor_implementation{
public:
    select_reactor_impl()
        : reactor_implementation()
        , dispatch_sets_()
        , wait_sets_()
        , ready_sets_()
        , demux_table_()
    {
    }
    int handle_events(std::chrono::microseconds *timeout) override;
    int register_handler(EventHandler* handler, Event_Type type) override;
    int unregister_handler(EventHandler *handler, Event_Type type) override;
    int register_handler(int handle, EventHandler *handler, Event_Type type) override;
    int unregister_handler(int handle, EventHandler *handler, Event_Type type) override;

    bool hasEvent(Event_Type type) const override {return demux_table_.hasEvent(type);}
private:
    int select(std::chrono::microseconds timeout);
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

//for timeout_dispatching
    int dispatchTimeoutHandlers();

private:
    //track handles that are currently ready for dispatch
    select_reactor_handle_sets dispatch_sets_;
    //track handles that are waited for by <select>
    select_reactor_handle_sets wait_sets_;

    //TODO track handles that are currently suspended
    // select_reactor_handle_sets suspend_set_;

    //TODO handles in ready_sets_ will be dispatched before select
    //track handles we are interested in for various events that must be dispatched without going through <select>
    select_reactor_handle_sets ready_sets_;
    select_demultiplex_table demux_table_;
};
}

#endif //_UNP_SELECT_REACTOR_IMPL_H_
