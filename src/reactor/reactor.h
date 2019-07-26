#ifndef _UNP_REACTOR_H_
#define _UNP_REACTOR_H_

#include <memory>
#include <atomic>
#include "reactor_implementation.h"
#include "reactor/EventFD.h"

namespace reactor {

enum class ReactorType
{
    POLL, EPOLL, SELECT
};

enum class ReactorStatus : uint
{
    RUNNING = 1, SUSPENDED = 2, SUSPENDING = 3
};

class Reactor{
public:
    using Event_Type = EventHandler::Event_Type;
    //should I alloc the memory of reactor_impl, new it in this constructor
    Reactor(reactor_implementation* reactor_impl = nullptr, bool needWakeUp = false)
        : reactor_impl_(reactor_impl)
        , status_{}
    {
        status_ = ReactorStatus::SUSPENDED;
        if (needWakeUp) //connector and epoll do not need weakup, poll select need it
        {
            eventFd_ptr_ = std::make_shared<EventFD>();
            eventFd_ptr_->registerInto(*this);
        }
    }
    virtual ~Reactor(){}

    virtual int register_handler(EventHandler* handler, Event_Type type){
        auto ret = reactor_impl_->register_handler(handler, type);
        if(ret == 0) handler->isRegistered_ = true;
        return ret;
    }
    virtual int register_handler(int handle, EventHandler *handler, Event_Type type) {
        int ret = reactor_impl_->register_handler(handle, handler, type);
        if(ret == 0) handler->isRegistered_ = true;

        if(!eventFd_ptr_)
            return ret;

        // 如果是event fd的read事件，那么不唤醒
        // 如果是event fd的write事件， 如果没有event fd的write事件就唤醒，有event fd的write就不唤醒
        if(handle == eventFd_ptr_->getEventFD() &&
            type == EventHandler::WRITE_EVENT &&
            reactor_impl_->register_handler(handle, handler, type) == -1 &&
            reactor_impl_->isWaiting())
        {
            LOG(INFO) << "eventfd write event";
            eventFd_ptr_->wakeup();
        }

        if(handle != eventFd_ptr_->getEventFD() && reactor_impl_->isWaiting())
        {
            LOG(INFO) << "current handle is not the event fd... the reactor will be waked up handle: " << handle;
            wakeup();
        }
        return ret;
    }
    virtual int unregister_handler(EventHandler *handler, Event_Type type) {
        auto ret = reactor_impl_->unregister_handler(handler, type);
        if(ret == 0) handler->isRegistered_ = false;
        return ret;
    }
    virtual int unregister_handler(int handle, EventHandler *handler, Event_Type type) {
        auto ret = reactor_impl_->unregister_handler(handle, handler, type);
        if(ret == 0) handler->isRegistered_ = false;
        return ret;
    }

    int handle_events(std::chrono::microseconds *timeout = nullptr){
        if(status_ == ReactorStatus::SUSPENDING) 
        {
            status_ = ReactorStatus::SUSPENDED;
            return -1;
        }
        return reactor_impl_->handle_events(timeout);
    }

    bool hasEvent(EventHandler::Event_Type type)
    {
        return reactor_impl_->hasEvent(type);
    }

    //suspend operation could have some delay, cause we have to wait for the reactor to finish last event handling
    void suspend()
    {
        status_ = ReactorStatus::SUSPENDING;
        wakeup();
    }

    void start()
    {
        status_ = ReactorStatus::RUNNING;
    }

    void wakeup() { eventFd_ptr_->wakeup(); }
    ReactorStatus reactorStatus() const {return status_;} 
private:
    std::shared_ptr<reactor_implementation> reactor_impl_;
    std::shared_ptr<EventFD> eventFd_ptr_;
    ReactorStatus status_;
    
};

} // Reactor

#endif // _UNP_REACTOR_H_
