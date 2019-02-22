#ifndef _UNP_POLL_REACTOR_IMPL_H_
#define _UNP_POLL_REACTOR_IMPL_H_

#include "server/reactor/reactor_implementation.h"
#include "server/util/easylogging++.h"
#include "server/reactor/select_reactor_impl.h"
#include "boost/shared_ptr.hpp"
#include <poll.h>
namespace reactor{

class poll_demultiplex_table {
public:
    using Event_Type = event_handler::Event_Type;
    poll_demultiplex_table() : demultiplex_table_(){}
    event_handler* get_handler(int handle, Event_Type type) const {
        return demultiplex_table_.get_handler(handle, type);
    }
    const std::vector<select_event_tuple>& get_event_vector() const { 
        return demultiplex_table_.get_event_vector(); 
    }
    int bind(int handle, event_handler* handler, Event_Type type){
        return demultiplex_table_.bind(handle, handler, type);
    }
    //unbind掉绑定到这个handle的所有事件处理器
    int unbind(int handle){
        return demultiplex_table_.unbind(handle);
    }
    int unbind(int handle, const event_handler* handler, Event_Type type){
        return demultiplex_table_.unbind(handle, handler, type);
    }
private:
    select_demultiplex_table demultiplex_table_;
};

class poll_reactor_impl : public reactor_implementation{
public:
    poll_reactor_impl();
    ~poll_reactor_impl();
    void handle_events(std::chrono::microseconds *timeout) override;
    int register_handler(event_handler* handler, Event_Type type) override;
    int register_handler(int handle, event_handler *handler, Event_Type type) override;
    int unregister_handler(event_handler *handler, Event_Type type) override;
    int unregister_handler(int handle, event_handler *handler, Event_Type type) override;

    poll_reactor_impl(const poll_reactor_impl&) = delete;
    poll_reactor_impl& operator=(const poll_reactor_impl&) = delete;
private:
    std::vector<struct pollfd>  wait_pfds_;
    poll_demultiplex_table      demux_table_;
};

class epoll_reactor_impl : public reactor_implementation 
{
public:
    using epoll_demultiplex_table = poll_demultiplex_table;
    epoll_reactor_impl();
    ~epoll_reactor_impl();
    void handle_events(std::chrono::microseconds *timeout) override;
    int register_handler(event_handler* handler, Event_Type type) override;
    int register_handler(int handle, event_handler *handler, Event_Type type) override;
    int unregister_handler(event_handler *handler, Event_Type type) override;
    int unregister_handler(int handle, event_handler *handler, Event_Type type) override;

    epoll_reactor_impl(const epoll_reactor_impl&) = delete;
    epoll_reactor_impl& operator=(const epoll_reactor_impl&) = delete;
private:
    epoll_demultiplex_table         demux_table_;
};

}
#endif //_UNP_POLL_REACTOR_IMPL_H_
