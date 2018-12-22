#ifndef _UNP_REACTOR_H_
#define _UNP_REACTOR_H_

#include <boost/shared_ptr.hpp>
#include "reactor_implementation.h"

namespace reactor {
class Reactor{
public:
    using Event_Type = event_handler::Event_Type;
    //should I alloc the memory of reactor_impl, new it in this constructor
    Reactor(reactor_implementation* reactor_impl = 0) : reactor_impl_(reactor_impl){}
    virtual ~Reactor(){}
    virtual void register_handler(event_handler* handler, Event_Type type){
        reactor_impl_->register_handler(handler, type);
    }
    virtual void register_handler(int handle, event_handler *handler, Event_Type type) {
        reactor_impl_->register_handler(handle, handler, type);
    }
    virtual void unregister_handler(event_handler *handler, Event_Type type) {
        reactor_impl_->unregister_handler(handler, type);
    }
    virtual void unregister_handler(int handle, event_handler *handler, Event_Type type) {
        reactor_impl_->unregister_handler(handle, handler, type);
    }
    void handle_events(std::chrono::microseconds *timeout = 0){
        reactor_impl_->handle_events(timeout);
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
    //static shared_ptr, means that this object will not be deleted
    // static boost::shared_ptr<Reactor> reactor_ptr_;
};

} // Reactor

#endif // _UNP_REACTOR_H_
