#ifndef _UNP_REACTOR_H_
#define _UNP_REACTOR_H_

#include <boost/shared_ptr.hpp>
#include "reactor_implementation.h"
#include "server/reactor/EventFD.h"

namespace reactor {
class Reactor{
public:
    using Event_Type = event_handler::Event_Type;
    //should I alloc the memory of reactor_impl, new it in this constructor
    Reactor(reactor_implementation* reactor_impl = 0) 
		: reactor_impl_(reactor_impl)
		, eventFd_()
	{
		eventFd_.registerInto(*this);
	}
    virtual ~Reactor(){}

    //此4个方法都是对wait_fds操作
    //TODO在多线程环境下，都是由acceptor的线程操作的, 而handle_events中肯定需要读取该wait_fds ，因此可能需要对wait_fds加锁
    //可能第一线程正在修改该wait_fds,因此它正处于中间状态，此时不应该对其调用poll 或者select
    virtual int register_handler(event_handler* handler, Event_Type type){
        return reactor_impl_->register_handler(handler, type);
    }
    virtual int register_handler(int handle, event_handler *handler, Event_Type type) {
        int ret = reactor_impl_->register_handler(handle, handler, type);
        if(reactor_impl_->isWaiting())
		    eventFd_.wakeup();
		return ret;
    }
    virtual int unregister_handler(event_handler *handler, Event_Type type) {
        return reactor_impl_->unregister_handler(handler, type);
    }
    virtual int unregister_handler(int handle, event_handler *handler, Event_Type type) {
        return reactor_impl_->unregister_handler(handle, handler, type);
    }
    int handle_events(std::chrono::microseconds *timeout = 0){
        return reactor_impl_->handle_events(timeout);
    }
    // static boost::shared_ptr<Reactor> instance(){
    //     using boost::shared_ptr;
    //     if(Reactor::reactor_ptr_.get() == 0){
    //         Reactor::reactor_ptr_ = shared_ptr<Reactor>{new Reactor{0}};
    //     }
    //     return Reactor::reactor_ptr_;
    // }
private:
    boost::shared_ptr<reactor_implementation> reactor_impl_;
	EventFD eventFd_;
    //static shared_ptr, means that this object will not be deleted
    // static boost::shared_ptr<Reactor> reactor_ptr_;
};

} // Reactor

#endif // _UNP_REACTOR_H_
