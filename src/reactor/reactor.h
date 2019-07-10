#ifndef _UNP_REACTOR_H_
#define _UNP_REACTOR_H_

#include <memory>
#include "reactor_implementation.h"
#include "reactor/EventFD.h"

namespace reactor {

enum class ReactorType
{
    POLL, EPOLL, SELECT
};

class Reactor{
public:
    using Event_Type = EventHandler::Event_Type;
    //should I alloc the memory of reactor_impl, new it in this constructor
    Reactor(reactor_implementation* reactor_impl = 0, bool needWakeUp = false)
		: reactor_impl_(reactor_impl)
	{
        if (needWakeUp) //connector and epoll do not need weakup, poll select need it
        {
            eventFd_ptr_ = std::make_shared<EventFD>();
            eventFd_ptr_->registerInto(*this);
        }
	}
    virtual ~Reactor(){}

    virtual int register_handler(EventHandler* handler, Event_Type type){
        return reactor_impl_->register_handler(handler, type);
    }
    virtual int register_handler(int handle, EventHandler *handler, Event_Type type) {
        int ret = reactor_impl_->register_handler(handle, handler, type);

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
            eventFd_ptr_->wakeup();
        }
		return ret;
    }
    virtual int unregister_handler(EventHandler *handler, Event_Type type) {
        return reactor_impl_->unregister_handler(handler, type);
    }
    virtual int unregister_handler(int handle, EventHandler *handler, Event_Type type) {
        return reactor_impl_->unregister_handler(handle, handler, type);
    }
    int handle_events(std::chrono::microseconds *timeout = 0){
        return reactor_impl_->handle_events(timeout);
    }

    bool hasEvent(EventHandler::Event_Type type)
    {
        return reactor_impl_->hasEvent(type);
    }
private:
    std::shared_ptr<reactor_implementation> reactor_impl_;
	std::shared_ptr<EventFD> eventFd_ptr_;
};

} // Reactor

#endif // _UNP_REACTOR_H_
