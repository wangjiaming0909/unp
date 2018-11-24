#ifndef _UNP_REACTOR_H_
#define _UNP_REACTOR_H_

#include <boost/shared_ptr.hpp>
#include "reactor_implementation.h"

namespace reactor {
class reactor{
public:
    //should I alloc the memory of reactor_impl, new it in this constructor
    reactor(reactor_implementation* reactor_impl = 0) : reactor_impl_(reactor_impl){}
    virtual ~reactor(){}
    using Event_Type = event_handler::Event_Type;
    virtual void register_handler(event_handler* handler, Event_Type type){
        reactor_impl_->register_handler(handler, type);
    }
    // virtual void register_handler(int handle, event_handler *handler, Event_Type type) {
    //     reactor_impl_->register_handler(handle, handler, type);
    // }
    virtual void remove_handler(event_handler *handler, Event_Type type) {
        reactor_impl_->remove_handler(handler, type);
    }
    // virtual void remove_handler(int handle, event_handler *handler, Event_Type type) {
    //     reactor_impl_->remove_handler(handle, handler, type);
    // }
    void handle_events(std::chrono::microseconds *timeout = 0){
        reactor_impl_->handle_events(timeout);
    }
    static boost::shared_ptr<reactor> instance(){
        using boost::shared_ptr;
        if(reactor::reactor_ptr_.get() == 0){
            reactor::reactor_ptr_ = shared_ptr<reactor>{new reactor{0}};
        }
        return reactor::reactor_ptr_;
    }
private:
    boost::shared_ptr<reactor_implementation> reactor_impl_;
    static boost::shared_ptr<reactor> reactor_ptr_;
};

boost::shared_ptr<reactor> reactor::reactor_ptr_ = 0;


} // reactor

#endif // _UNP_REACTOR_H_
