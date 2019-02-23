#include "server/reactor/poll_reactor_impl.h"

using namespace reactor;

poll_reactor_impl::poll_reactor_impl() : wait_pfds_(), demux_table_()
{

}

poll_reactor_impl::~poll_reactor_impl(){}

int poll_reactor_impl::register_handler(event_handler* handler, Event_Type type) 
{ 
    (void)handler;
    (void)type;
}

int poll_reactor_impl::unregister_handler(event_handler *handler, Event_Type type) 
{ 
    (void)handler;
    (void)type;
}

void poll_reactor_impl::handle_events(std::chrono::microseconds *timeout)
{
    int n = 0;
    if(this->poll(timeout) > 0)
    {
        LOG(INFO) << n << "handles ready...";
        int n = this->dispatch(n);
        if(n != 0) LOG(WARNING) << "dispatch returned " << n;
    }
    LOG(WARNING) << "Poll returned 0 or -1" << strerror(errno);
}

int  poll_reactor_impl::register_handler(int handle, event_handler *handler, Event_Type type)
{
    LOG(INFO) << "Registering handler, handle: " << handle << " event: " << event_type_to_string(type);
    if(handle == INVALID_HANDLE || handler == 0 || type == event_handler::NONE){
        LOG(ERROR) << "Handle error or registered type error...";
        return -1;
    }

    //already existed in the table
    if(demux_table_.get_handler(handle, type) != 0)
    {
        LOG(WARNING) << "Already existed in the demultiplex table, handle: " 
        << handle << " event: " << event_type_to_string(type);
    }

    struct pollfd pfd{};
    pfd.fd = handle;
    pfd.events |= reactor_event_to_poll_event(type, USING_POLL);
    pfd.revents = 0;

    //TODO 虽然 demultiplex table 中没有这个 handle， 但是 pollfd 的 vector 中可能有
    wait_pfds_.push_back(pfd);

    //bind to the demultiplex table
    return this->demux_table_.bind(handle, handler, type);
}

int poll_reactor_impl::unregister_handler(int handle, event_handler *handler, Event_Type type)
{
    LOG(INFO) << "Unregistering handler, handle: " << handle << " event: " << event_type_to_string(type);
    if(handle == INVALID_HANDLE || handler == 0 || type == event_handler::NONE){
        LOG(ERROR) << "Handle error or unregistered type error...";
        return -1;
    }

    //didn't find the handle and handler
    if(demux_table_.get_handler(handle, type) == 0)
    {
        LOG(WARNING) << "Can't unregister, didn't find the handle: " 
            << handle << " event: " << event_type_to_string(type);
        return -1;
    }

    for(int i = 0; i < wait_pfds_.size(); i++)
    {
        struct pollfd& pfd_r = wait_pfds_[i];
        if(pfd_r.fd == handle && 
            (pfd_r.events & reactor_event_to_poll_event(type, USING_POLL)))
        {
            wait_pfds_.erase(wait_pfds_.begin() + i);
        }
    }

    return demux_table_.unbind(handle, handler, type);
}

int poll_reactor_impl::poll(std::chrono::microseconds* timeout)
{
    LOG(INFO) << "Preparing to poll...";

    if(wait_pfds_.size() == 0)
    {
        LOG(WARNING) << "no wait fds...";
        return -1;
    } 
    int ret = ::poll(&wait_pfds_[0], wait_pfds_.size(), timeout->count());
}

int poll_reactor_impl::dispatch(int active_handle_count)
{

}

epoll_reactor_impl::epoll_reactor_impl()
{

}

epoll_reactor_impl::~epoll_reactor_impl()
{

}

void epoll_reactor_impl::handle_events(std::chrono::microseconds *timeout)
{

}

int epoll_reactor_impl::register_handler(event_handler* handler, Event_Type type)
{

}

int epoll_reactor_impl::register_handler(int handle, event_handler *handler, Event_Type type)
{

}

int epoll_reactor_impl::unregister_handler(event_handler *handler, Event_Type type)
{

}

int epoll_reactor_impl::unregister_handler(int handle, event_handler *handler, Event_Type type)
{

}