#ifndef _UNP_POLL_REACTOR_IMPL_H_
#define _UNP_POLL_REACTOR_IMPL_H_

#include <poll.h>
#include <chrono>
#include <mutex>
#include <sys/epoll.h>

#include "reactor/reactor_implementation.h"
#include "util/easylogging++.h"
#include "reactor/poll_demultiplex_table.h"

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

}
#endif //_UNP_POLL_REACTOR_IMPL_H_
