#include "server/reactor/epoll_reactor_impl.h"

namespace reactor
{

epoll_reactor_impl::epoll_reactor_impl() 
    : fd_count_(0)
    , epoll_fd_(-1)
    , cur_event_()
    , ret_events_()
    , demux_table_()
    , mutex_()
    , epoller_()
{
    this->open();
}

epoll_reactor_impl::~epoll_reactor_impl()
{
    epoller_.close();
}

int epoll_reactor_impl::open()
{
    return epoller_.open();
}

int epoll_reactor_impl::handle_events(std::chrono::microseconds *timeout)
{
    int n = 0;
    if(timeout != 0)
    {
        std::chrono::milliseconds timeout_milli = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);

        n = this->epoll_wait(timeout_milli.count());
    }
    else
    {
        n = this->epoll_wait(-1);
    }
    
    if(n == 0) 
    {
        LOG(WARNING) << "No events ready...";
        ::sleep(2);
    }
    else if(n < 0)
    {
        LOG(WARNING) << "Epoll_wait returned 0 or -1" << strerror(errno);
        return -1;
    }
    else if(n > 0)
    {
        LOG(INFO) << n << " handle(s) ready...";
        n = this->dispatch(n);
    }
    return 0;
}

int epoll_reactor_impl::register_handler(event_handler* handler, Event_Type type)
{
    (void)handler;
    (void)type;
	return 0;
}

int epoll_reactor_impl::unregister_handler(event_handler *handler, Event_Type type)
{
    (void)handler;
    (void)type;
	return 0;
}

int epoll_reactor_impl::register_handler(int handle, event_handler *handler, Event_Type type)
{
    // std::lock_guard<std::mutex> guard(mutex_);

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

    int ret = epoller_.add(handle, reactor_event_to_poll_event(type, USING_EPOLL));

    // memset(&cur_event_, 0, sizeof(struct epoll_event));

    // if(type & event_handler::READ_EVENT)
    // {

    // }
    // else if(type & event_handler::WRITE_EVENT)

    // cur_event_.events = reactor_event_to_poll_event(type, USING_EPOLL);
    // void *p_fd = &cur_event_.data.u64;
    // *(int*)p_fd = handle;
    // *(unsigned int*)(static_cast<char*>(p_fd) + 4) = type;
    // // cur_event_.data.u64 = (uint64_t)handle << 32 + type;


    // int ret = 0;
    // if(!demux_table_.has_handle(handle))
    //     ret = ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handle, &cur_event_);
    // else
    //     ret = ::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handle, &cur_event_);

    if(ret < 0)
    {
        LOG(ERROR) << "Epoll_ctl add handle error... " << strerror(errno);
        return -1;
    }

    ret = this->demux_table_.bind(handle, handler, type);
    if(ret == 0)
        fd_count_++;
    return ret;
}

int epoll_reactor_impl::unregister_handler(int handle, event_handler *handler, Event_Type type)
{
    // std::lock_guard<std::mutex> guard(mutex_);

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

    int ret = epoller_.del(handle, reactor_event_to_poll_event(type, USING_EPOLL));

    if(ret < 0)
    {
        LOG(ERROR) << "Epoll_ctl del handle error... " << strerror(errno);
        return -1;
    }

    ret = this->demux_table_.unbind(handle, handler, type);
    if(ret == 0) 
        fd_count_--;
    return ret;
}

int epoll_reactor_impl::epoll_wait(int milliseconds)
{
    LOG(INFO) << "Epoll waiting...";
    {
        // std::lock_guard<std::mutex> guard(mutex_);
        ret_events_.clear();
        ret_events_.resize(fd_count_);
    }

    // int ret = ::epoll_wait(epoll_fd_, &ret_events_[0], fd_count_, milliseconds == 0 ? -1 : milliseconds);
    int ret = epoller_.epoll_wait(&ret_events_[0], fd_count_, milliseconds == 0 ? -1 : milliseconds);

    if(ret < 0)
    {
        LOG(WARNING) << "Epoll error " << strerror(errno);
        return 0;
    }

    if(ret == 0 && milliseconds != 0)
    {
        LOG(WARNING) << "Poll timed out...";
        return 0;
    }

    return ret;
}

int epoll_reactor_impl::dispatch(int active_handle_count)
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

int epoll_reactor_impl::dispatch_io_handlers(int active_handles, int& handles_dispatched)
{
    (void)handles_dispatched;
    int remain = 0;
    remain = this->dispatch_io_epoll_sets(active_handles, 0, EPOLLIN, &event_handler::handle_input);

    if (remain > 0)
        remain = this->dispatch_io_epoll_sets(remain, 0, EPOLLOUT, &event_handler::handle_output);

    if(remain > 0)
        remain = this->dispatch_io_epoll_sets(remain, 0, EPOLLERR, &event_handler::handle_output);

    return remain;
}

int epoll_reactor_impl::dispatch_io_epoll_sets(int active_handles, int handles_dispatched, Event_Type type, HANDLER callback)
{
    epoll_event* ep_event_dispatching = nullptr;
    int current_fd = -1;
    int ret = -1;


    for(size_t i = 0; i < ret_events_.size(); i++)
    {
        if(active_handles - handles_dispatched <= 0)
            break;

        // mutex_.lock();

        ep_event_dispatching = &ret_events_[i];
        current_fd = ep_event_dispatching->data.fd;
        if(!(ep_event_dispatching->events & type))
        {
            // mutex_.unlock();
            continue;
        }
        
        handles_dispatched++;

        LOG(INFO) << "Dispatching handle: " << current_fd << " event: " << event_type_to_string(type);


        event_handler* handler = demux_table_.get_handler(current_fd, type);
        if(handler != nullptr) ret = (handler->*callback)(current_fd);

        // mutex_.unlock();

        if(ret < 0 && (handler != nullptr))
        {
            LOG(INFO) << "Unbinding handle: " << current_fd << " event: " << event_type_to_string(type);

            if(type == event_handler::READ_EVENT)
            {
                handler->close_read(current_fd);
            }else if(type == event_handler::WRITE_EVENT)
            {
                handler->close_write(current_fd);
            }
        } 
        else
        {
            LOG(INFO) <<"Keep listening on handle: " << current_fd << " event: " << event_type_to_string(type);
        }
        bool isHasHandle = demux_table_.has_handle(current_fd);
        if(!isHasHandle && (handler != nullptr))
        {
            handler->handle_close(current_fd);
        }
    }
    return active_handles - handles_dispatched;
}

}//namespace reactor