#include "server/reactor/poll_reactor_impl.h"
#include "boost/assert.hpp"

using namespace reactor;

poll_reactor_impl::poll_reactor_impl() : wait_pfds_(), demux_table_() { }

poll_reactor_impl::~poll_reactor_impl(){}

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
        return -1;
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

    for(size_t i = 0; i < wait_pfds_.size(); i++)
    {
        struct pollfd& pfd_r = wait_pfds_[i];
        if(pfd_r.fd == handle && 
            (pfd_r.events & reactor_event_to_poll_event(type, USING_POLL)))
        {
            wait_pfds_.erase(wait_pfds_.begin() + i);
            break;
        }
    }

    return demux_table_.unbind(handle, handler, type);
}

int poll_reactor_impl::handle_events(std::chrono::microseconds *timeout)
{
    int n = this->poll(timeout);

    if(n == 0) 
    {
        LOG(WARNING) << "No events ready...";
        ::sleep(2);
    }
    else if(n < 0)
    {
        LOG(WARNING) << "Poll returned 0 or -1" << strerror(errno);
        return -1;
    }
    else if(n > 0)
    {
        LOG(INFO) << n << " handle(s) ready...";
        n = this->dispatch(n);
    }
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

    isWaiting_ = true;

    std::vector<struct pollfd> waits{};
    for(auto& fd : wait_pfds_)
    {
        waits.push_back(fd);
    }

    int ret = ::poll(&waits[0], waits.size(), timeout == 0 ? -1 : timeout->count());

    for (size_t i = 0; i < waits.size(); i++)
    {
        wait_pfds_[i].revents = waits[i].revents;
    }

    isWaiting_ = false;

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
        LOG(WARNING) << "Dispatch io handlers error";
        return -1;
    }
    return 0;
}

//TODO handle remote close event
int poll_reactor_impl::dispatch_io_handlers(int active_handles, int& handles_dispatched)
{
    int ret = 0;
    ret = dispatch_io_sets(active_handles, handles_dispatched, POLLIN, &event_handler::handle_input);

    ret = ret && dispatch_io_sets(active_handles, handles_dispatched, POLLOUT, &event_handler::handle_output);

    ret = ret && dispatch_io_sets(active_handles, handles_dispatched, POLLERR, &event_handler::handle_output);
    return ret;
}

int poll_reactor_impl::dispatch_io_sets(int active_handles, int& handles_dispatched, Event_Type type, HANDLER callback)
{
    struct pollfd* pfd_dispatching = nullptr;
    int current_fd = -1;
    int ret = -1;
    for(size_t i = 0; i < wait_pfds_.size() && (active_handles > 0); i++)
    {
        pfd_dispatching = &wait_pfds_[i];
        current_fd = pfd_dispatching->fd;
        if(!(pfd_dispatching->revents & type))
            continue;
        
        handles_dispatched++;

        LOG(INFO) << "Dispatching handle: " << current_fd << " event: " << event_type_to_string(type);
        event_handler* handler = demux_table_.get_handler(current_fd, type);

        if(handler != nullptr) ret = (handler->*callback)(current_fd);
        active_handles--;

        if(ret < 0 && (handler != nullptr))
        {
            LOG(INFO) << "Unbinding handle: " << current_fd << " event: " << event_type_to_string(type);
            // this->unregister_handler(current_fd, handler, type);
            if(type == event_handler::READ_EVENT)
            {
                handler->close_read(current_fd);
            }
            else if(type == event_handler::WRITE_EVENT)
            {
                handler->close_write(current_fd);
            }
        } 
        else
        {
            LOG(INFO) << "Keep listening on handle: " << current_fd << " event: " << event_type_to_string(type);
        }
        auto iter = std::find_if(wait_pfds_.begin(), wait_pfds_.end(), 
            [current_fd](struct pollfd& pfd)
            {
                return (pfd.fd == current_fd);
            });
        //如果再次查找的時候, 已經到了最後位置了，說明已不存在當前handle的其他事件監聽了，因此，調用handle_close
        if((handler != nullptr) && iter == wait_pfds_.end())
        {
            handler->handle_close(current_fd);
        }
    }
    return active_handles;
}


int poll_reactor_impl::register_handler(event_handler* handler, Event_Type type) 
{ 
    (void)handler;
    (void)type;
	return 0;
}

int poll_reactor_impl::unregister_handler(event_handler *handler, Event_Type type) 
{ 
    (void)handler;
    (void)type;
	return 0;
}