#ifndef _UNP_REACTOR_H_
#define _UNP_REACTOR_H_

#include <memory>
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
        //TODO when to wake up need more attention
		/* 
		 * 最初注册eventfd的read事件时,不需要wakeup,因为它是第一个fd,且是第一个事件,还没有任何fd注册在上面
		 * 当有一个新的fd被注册上来时,此时reactor正等待的只有eventfd的read事件,因此需要唤醒,重新handle_events
		 * 唤醒的时候,wake up会向eventfd中写入一些数据
		 * 然后 enable_writing, 然而,此时还没有完成唤醒
		 * 因此enable_writing并不会被reactor发现(当过滤条件设置了只要是eventfd就不唤醒时),也就是唤醒失败,
		 * 因此过滤条件因包括eventfd的write事件,如此反复循环,并没有效果
		 */

        // 如果是event fd的read事件，那么不唤醒
        // 如果是event fd的write事件， 如果没有event fd的write事件就唤醒，有event fd的write就不唤醒
        if(handle == eventFd_.getEventFD() && 
            type == event_handler::WRITE_EVENT && 
            reactor_impl_->register_handler(handle, handler, type) == -1 &&
            reactor_impl_->isWaiting())
        {
            LOG(INFO) << "eventfd write event";
            eventFd_.wakeup();
        }

        if(handle != eventFd_.getEventFD() && reactor_impl_->isWaiting())
        {
            LOG(INFO) << "current handle is not the event fd... the reactor will be waked up handle: " << handle;
            eventFd_.wakeup();
        }

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
    std::shared_ptr<reactor_implementation> reactor_impl_;
	EventFD eventFd_;
    //static shared_ptr, means that this object will not be deleted
    // static boost::shared_ptr<Reactor> reactor_ptr_;
};

} // Reactor

#endif // _UNP_REACTOR_H_
