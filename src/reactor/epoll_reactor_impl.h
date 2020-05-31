#ifndef EPOLL_REACTOR_IMPL_H_
#define EPOLL_REACTOR_IMPL_H_

#include <mutex>
#include <sys/epoll.h>
#include "reactor/reactor_implementation.h"
#include "reactor/epoller.h"
#include "reactor/poll_demultiplex_table.h"


namespace reactor
{

class epoll_reactor_impl : public reactor_implementation
{
public:
    typedef int (EventHandler::*HANDLER)(int);
    //using epoll_demultiplex_table = poll_demultiplex_table;
    using epoll_demultiplex_table = PollEventRepo;
    epoll_reactor_impl();
    ~epoll_reactor_impl();
    int handle_events(std::chrono::microseconds *timeout) override;

    int register_handler(EventHandler* handler, Event_Type type) override;
    int register_handler(int handle, EventHandler *handler, Event_Type type) override;
    int unregister_handler(EventHandler *handler, Event_Type type) override;
    int unregister_handler(int handle, EventHandler *handler, Event_Type type) override;

    epoll_reactor_impl(const epoll_reactor_impl&) = delete;
    epoll_reactor_impl& operator=(const epoll_reactor_impl&) = delete;

    bool hasEvent(Event_Type) const override{return false;}
private:
    int open();
    //TODO epoll 的精度是 microseconds 么?
    int epoll_wait(int milliseconds);
    int dispatch(int active_handle_count);
    int dispatch_io_handlers(int active_handles, int& handles_dispatched);
    int dispatch_io_epoll_sets(int active_handles, int handles_dispatched, Event_Type type, HANDLER callback);
    int dispatch_timeout_events();

private:
    int                                             fd_count_;
    int                                             epoll_fd_;
    struct epoll_event                              cur_event_;
    std::vector<struct epoll_event>                 ret_events_;
    epoll_demultiplex_table                         demux_table_;
    std::mutex                                      mutex_;
    epoller                                         epoller_;
    std::chrono::steady_clock::time_point                cached_now_;
};

} //namespace reactor

#endif // EPOLL_REACTOR_IMPL_H_
