#include "server/reactor/poll_reactor_impl.h"

using namespace reactor;

short reactor::reactor_event_to_poll_event(event_handler::Event_Type type, int poll_or_epoll )
{
    short events = 0;

    if(type == event_handler::READ_EVENT || 
        type == event_handler::ACCEPT_EVENT || 
        type == event_handler::CONNECT_EVENT){
        if(poll_or_epoll == USING_POLL) 
            events |= POLLIN;
        else 
            events |= EPOLLIN;
    }
    if(type == event_handler::WRITE_EVENT || 
        type == event_handler::CONNECT_EVENT){
        if(poll_or_epoll == USING_EPOLL) 
            events |= POLLOUT;
        else 
            events |= EPOLLOUT;
    }
    return events;
        // NONE            = 0x000,
        // READ_EVENT      = POLLIN,
        // EXCEPT_EVENT    = POLLPRI,//0x2
        // WRITE_EVENT     = POLLOUT,//0x4
        // ACCEPT_EVENT    = 1 << 3,
        // TIMEOUT_EVENT   = 1 << 4,
        // SIGNAL_EVENT    = 1 << 5,
        // CLOSE_EVENT     = 1 << 6,
        // CONNECT_EVENT   = 1 << 7
}

int poll_event_repo::bind_new(Event_Type type, event_handler* handler)
{
    if(type == event_handler::NONE || handler == 0)
    {
        LOG(WARNING) << "can't bind NONE event or handler is nullptr";
        return -1;
    }

    types_and_handlers_.push_back(std::make_pair(type, handler));
    return 0;
}

int poll_event_repo::unbind(Event_Type type, const event_handler* handler)
{
    if(type == event_handler::NONE || handler == 0)
    {
        LOG(WARNING) << "can't unbind NONE event or handler can't be nullptr";
        return -1;
    }

    auto event_tuple_find = this->find(type);
    if(event_tuple_find == types_and_handlers_.end())
    {
        LOG(WARNING) << "can't unbind, no this type of event";
        return -1;
    }

    if(event_tuple_find->second != handler)
    {
        LOG(WARNING) << "the handler you unbinded is not the same as you provided";
        return -1;
    }

    types_and_handlers_.erase(event_tuple_find);
    return 0;
}

event_handler* poll_event_repo::get_handler(Event_Type type) const 
{
    auto iter = this->find(type);
    if(iter == types_and_handlers_.end())
    {
        // LOG(WARNING) << "can't unbind, no this type of event";
        return nullptr;
    }
    return iter->second;
}

std::vector<poll_event_repo::event_tuple>::const_iterator poll_event_repo::find(Event_Type type) const
{
    int size = types_and_handlers_.size();
    return std::find_if(types_and_handlers_.begin(), types_and_handlers_.end(), 
        [&type] (const event_tuple& tuple) 
        {
            if((tuple.first == event_handler::READ_EVENT || tuple.first == event_handler::ACCEPT_EVENT) 
                && type == POLLIN)
                return true;
            if((tuple.first == event_handler::WRITE_EVENT || tuple.first == event_handler::CONNECT_EVENT)
                && type == POLLOUT)
                return true;
            if((tuple.first == event_handler::EXCEPT_EVENT && type == POLLPRI)) return true;
        });
}

poll_reactor_impl::poll_reactor_impl() : wait_pfds_(), demux_table_() { }

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

int poll_reactor_impl::handle_events(std::chrono::microseconds *timeout)
{
    int n = this->poll(timeout);
    if(n == 0) ::sleep(2);
    if(n < 0)
    {
        LOG(WARNING) << "Poll returned 0 or -1" << strerror(errno);
        return -1;
    }
    LOG(INFO) << n << " handle(s) ready...";
    n = this->dispatch(n);
    if(n != 0) LOG(WARNING) << "dispatch returned " << n;
    return 0;
}

int poll_reactor_impl::poll(std::chrono::microseconds* timeout)
{
    LOG(INFO) << "Preparing to poll...";

    if(wait_pfds_.size() == 0)
    {
        LOG(WARNING) << "no wait pollfds...";
        return -1;
    } 
    int ret = ::poll(&wait_pfds_[0], wait_pfds_.size(), timeout == 0 ? -1 : timeout->count());

    if(ret < 0)
    {
        LOG(WARNING) << "Poll error " << strerror(errno);
        return -1;
    }

    if(ret == 0 && timeout != 0 && timeout->count() != 0)
    {
        LOG(WARNING) << "Poll timed out...";
        return 0;
    }

    return ret;
}

int poll_reactor_impl::dispatch(int active_handle_count)
{
    int handles_dispatched = 0;
    int ret = this->dispatch_io_handlers(active_handle_count, handles_dispatched);
    if(ret != 0)
    {
        LOG(WARNING) << "dispatch io handlers error";
        return -1;
    }
    return 0;
}

//TODO handle remote close event
int poll_reactor_impl::dispatch_io_handlers(int active_handles, int& handles_dispatched)
{
    int ret = 0;
    ret = dispatch_io_sets(active_handles, handles_dispatched, POLLIN, &event_handler::handle_input);

    ret = dispatch_io_sets(active_handles, handles_dispatched, POLLOUT, &event_handler::handle_output);

    ret = dispatch_io_sets(active_handles, handles_dispatched, POLLERR, &event_handler::handle_output);
    return ret;
}

int poll_reactor_impl::dispatch_io_sets(int active_handles, int& handles_dispatched, Event_Type type, HANDLER callback)
{
    struct pollfd* pfd_dispatching = nullptr;
    int current_fd = -1;
    int ret = 0;
    for(int i = 0; i < wait_pfds_.size(); i++)
    {
        pfd_dispatching = &wait_pfds_[i];
        current_fd = pfd_dispatching->fd;
        if(!(pfd_dispatching->revents & type))
            continue;

        
        handles_dispatched++;

        LOG(INFO) << "Dispatching handle: " << current_fd << " event: " << event_type_to_string(type);
        event_handler* handler = demux_table_.get_handler(current_fd, type);
        if(handler == 0) return -1;
        ret = (handler->*callback)(current_fd);

        if(ret < 0)
        {
            LOG(INFO) << "unbinding handle: " << current_fd << " event: " << event_type_to_string(type);
            this->demux_table_.unbind(current_fd, handler, type);
            auto iter = std::find_if(wait_pfds_.begin(), wait_pfds_.end(), 
                [&type, current_fd](struct pollfd& pfd)
                {
                    return (pfd.fd == current_fd) && 
                    (pfd.revents & reactor::reactor_event_to_poll_event(type, USING_POLL));
                });
            wait_pfds_.erase(iter);
            iter = std::find_if(wait_pfds_.begin(), wait_pfds_.end(), 
                [current_fd](struct pollfd& pfd)
                {
                    return (pfd.fd == current_fd);
                });
            if(iter == wait_pfds_.end())
            {
                handler->handle_close(current_fd);
            }
        } 
        else
        {
            LOG(INFO) <<"keep listening on handle: " << current_fd << " event: " << event_type_to_string(type);
        }
    }
    return 0;
}

epoll_reactor_impl::epoll_reactor_impl()
{

}

epoll_reactor_impl::~epoll_reactor_impl()
{

}

int epoll_reactor_impl::handle_events(std::chrono::microseconds *timeout)
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
