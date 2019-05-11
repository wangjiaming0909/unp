#ifndef _UNP_POLL_REACTOR_IMPL_H_
#define _UNP_POLL_REACTOR_IMPL_H_

#include "server/reactor/reactor_implementation.h"
#include "server/util/easylogging++.h"
#include "server/reactor/select_reactor_impl.h"
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include <poll.h>
#include <sys/epoll.h>
#include <chrono>
#include <mutex>
#include "server/reactor/epoller.h"

using namespace std::chrono_literals;

namespace reactor{

struct event_pair
{
    event_pair() : handle(0), handler(nullptr){}
    event_pair(unsigned int handle, event_handler* handler)
        : handle(handle), handler(handler){}
    unsigned int handle;
    event_handler* handler;
};

class poll_event_repo {
public:
    // using event_tuple = std::pair<unsigned int, event_handler*>;
    using event_tuple = event_pair;
    using Event_Type = event_handler::Event_Type;
    poll_event_repo() : types_and_handlers_() {}
    ~poll_event_repo() {}
    int bind_new(Event_Type type, event_handler* handler);
    int unbind(Event_Type type, const event_handler* handler);
    void clear() {types_and_handlers_.clear();}
    event_handler* get_handler(Event_Type type) const ;
    int handle_count() const {return types_and_handlers_.size();};

private:
    std::vector<event_tuple>::const_iterator find(Event_Type type) const;
private:
    std::vector<event_tuple> types_and_handlers_;
};

class poll_demultiplex_table {
public:
    using Event_Type = event_handler::Event_Type;
    poll_demultiplex_table() : table_(), size_(0){}
    event_handler* get_handler(int handle, Event_Type type) const 
    {
        if(table_.size() <= static_cast<size_t>(handle))
            return nullptr;
        return table_[handle].get_handler(type);
    }

    bool has_handle(int handle) const 
    {
        return table_.size() > static_cast<size_t>(handle) && table_[handle].handle_count() > 0;
    }

    const std::vector<poll_event_repo>& get_event_vector() const 
    { 
        return table_;
    }

    int bind(int handle, event_handler* handler, Event_Type type){
        if(static_cast<size_t>(handle) >= table_.size())
        {
            LOG(INFO) << "resizing table to size: " << handle + 10;
            table_.resize(handle + 10);
        }
        size_++;
        LOG(INFO) << "demultiplex table size: " << size_;
        LOG(INFO) << "current handle is: " << handle;
        return table_[handle].bind_new(type, handler);
    }

    //unbind掉绑定到这个handle的所有事件处理器
    int unbind(int handle){
        int handle_count = table_[handle].handle_count();
        if(handle_count <= 0)
        {
            LOG(WARNING) << "No handler bind to handle: " << handle;
            return -1;
        }
        table_[handle].clear();
        size_ -= handle_count;
        return 0;
    }
    int unbind(int handle, const event_handler* handler, Event_Type type){
        return table_[handle].unbind(type, handler);
    }
private:
    std::vector<poll_event_repo>    table_;
    int                             size_;
};


class poll_reactor_impl : public reactor_implementation{
public:
    typedef int (event_handler::*HANDLER)(int);
    poll_reactor_impl();
    ~poll_reactor_impl();
    int handle_events(std::chrono::microseconds *timeout) override;
    int register_handler(event_handler* handler, Event_Type type) override;
    int register_handler(int handle, event_handler *handler, Event_Type type) override;
    int unregister_handler(event_handler *handler, Event_Type type) override;
    int unregister_handler(int handle, event_handler *handler, Event_Type type) override;

    poll_reactor_impl(const poll_reactor_impl&) = delete;
    poll_reactor_impl& operator=(const poll_reactor_impl&) = delete;
private:
    int poll(std::chrono::microseconds* timeout);
    int dispatch(int active_handle_count);
    int dispatch_io_handlers(int active_handles, int& handles_dispatched);
    int dispatch_io_sets(int active_handles, int& handles_dispatched, Event_Type type, HANDLER callback);
private:
    std::vector<struct pollfd>  wait_pfds_;
    poll_demultiplex_table      demux_table_;
};

class epoll_reactor_impl : public reactor_implementation 
{
public:
    typedef int (event_handler::*HANDLER)(int);
    using epoll_demultiplex_table = poll_demultiplex_table;
    epoll_reactor_impl();
    ~epoll_reactor_impl();
    int handle_events(std::chrono::microseconds *timeout) override;

    int register_handler(event_handler* handler, Event_Type type) override;
    int register_handler(int handle, event_handler *handler, Event_Type type) override;
    int unregister_handler(event_handler *handler, Event_Type type) override;
    int unregister_handler(int handle, event_handler *handler, Event_Type type) override;

    epoll_reactor_impl(const epoll_reactor_impl&) = delete;
    epoll_reactor_impl& operator=(const epoll_reactor_impl&) = delete;

private:
    int open();
    //TODO epoll 的精度是 microseconds 么?
    int epoll_wait(int milliseconds);
    int dispatch(int active_handle_count);
    int dispatch_io_handlers(int active_handles, int& handles_dispatched);
    int dispatch_io_epoll_sets(int active_handles, int& handles_dispatched, Event_Type type, HANDLER callback);

private:
    int                                 fd_count_;
    int                                 epoll_fd_;
    struct epoll_event                  cur_event_;
    std::vector<struct epoll_event>     ret_events_;
    epoll_demultiplex_table             demux_table_;
    std::mutex                          mutex_;
    epoller                             epoller_;
};

static const int USING_POLL = 0;
static const int USING_EPOLL = 1;

short reactor_event_to_poll_event(event_handler::Event_Type type, int poll_or_epoll );

}
#endif //_UNP_POLL_REACTOR_IMPL_H_
