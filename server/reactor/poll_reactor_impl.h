#ifndef _UNP_POLL_REACTOR_IMPL_H_
#define _UNP_POLL_REACTOR_IMPL_H_

#include <poll.h>
#include <sys/epoll.h>
#include <chrono>
#include <mutex>

#include "server/reactor/reactor_implementation.h"
#include "server/util/easylogging++.h"
#include "server/reactor/epoller.h"
#include "server/reactor/poll_demultiplex_table.h"

using namespace std::chrono_literals;

namespace reactor{

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
