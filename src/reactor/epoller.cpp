#include "reactor/epoller.h"
#include "util/easylogging++.h"
#include <cassert>

#include <unistd.h>

namespace reactor{


epoller::epoller() : epoll_fd_(0), handle_event_map_()
{
}

epoller::~epoller()
{
}

int epoller::epoll_wait(epoll_event* ret_events, int max_events, int timeout)
{
    return ::epoll_wait(epoll_fd_, ret_events, max_events, timeout);
}

int epoller::open()
{
    epoll_fd_ = ::epoll_create1(0);

    if(epoll_fd_ < 0)
    {
        LOG(ERROR) << "Open epoll_fd error... " << strerror(errno);
        return -1;
    }
    return 0;
}

int epoller::add(int handle, uint32_t event)
{
    handle_event_map_t::size_type count = handle_event_map_.count(handle);
    int operation = -1;

    //存在此handle， 并且存在此event
    if(count != 0 && handle_event_map_[handle]->events & event)
    {
        // LOG(ERROR) << "existed handle and event handle: " << handle << " event: " << event;
        return -1;
    }

    //存在此handle， 但是不存在此event
    if(count != 0)
    {
        handle_event_map_[handle]->events |= event;
        operation = EPOLL_CTL_MOD;
    }
    else {
        //handle 也不存在
        epoll_event_ptr_t epollEvent = std::make_shared<epoll_event>();
        epollEvent->events = event;

        void *p_fd = &epollEvent->data.u64;
        *(int*)p_fd = handle;

        handle_event_map_.insert({handle, epollEvent});
        operation = EPOLL_CTL_ADD;
    }
    if(::epoll_ctl(epoll_fd_, operation, handle, handle_event_map_[handle].get()) != 0)
    {
        // LOG(WARNING) << "epoll_ctl error" << strerror(errno);
        return -1;
    }
    return 0;
}

int epoller::del(int handle, uint32_t event)
{
    handle_event_map_t::size_type count = handle_event_map_.count(handle);
    int operation = -1;

    // 不存在 fd或者不存event
    if(count == 0 || 
        (count == 1 && !(handle_event_map_[handle]->events & event)))
    {
        // LOG(ERROR) << "no this handle or event in  epoll events";
        return -1;
    }

    //exist 
    uint32_t* current_event = &handle_event_map_[handle]->events;
    operation = EPOLL_CTL_MOD;
    epoll_event ev = *(handle_event_map_[handle]);
    ev.events &= ~event;

    //no more events
    if(::epoll_ctl(epoll_fd_, operation, handle, &ev) != 0)
    {
        LOG(ERROR) << "epoll ctl error " << strerror(errno);
        return -1;
    }
    *current_event &= ~event;
    if(*current_event == 0)
    {
        operation = EPOLL_CTL_DEL;
        if(::epoll_ctl(epoll_fd_, operation, handle, handle_event_map_[handle].get()) != 0)
        {
            LOG(ERROR) << "delete handle from epoll error... " << strerror(errno);
            return -1;
        }
        handle_event_map_.erase(handle);
    }

    return 0;
}

bool epoller::has(int handle, uint32_t event)
{
    handle_event_map_t::size_type count = handle_event_map_.count(handle);
    if(count == 0) return false;

    //count != 0
    assert(count == 1);
    return handle_event_map_[handle]->events & event;
}

int epoller::close()
{
    int ret = ::close(epoll_fd_);
    if(ret < 0)
    {
        LOG(ERROR) << "Close epoll_fd error... " << strerror(errno);
    }
    return ret;
}


}//reactor