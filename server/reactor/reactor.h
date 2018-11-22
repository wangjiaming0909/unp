#ifndef _UNP_REACTOR_H_
#define _UNP_REACTOR_H_

#include <chrono>
#include "event_handler.h"
#include "demultiplex_table.h"
#include <boost/shared_ptr.hpp>

namespace reactor
{
class event_handler;
class reactor_implementation;
class reactor{
public:
    //should I alloc the memory of reactor_impl, new it in the this constructor
    reactor(reactor_implementation* reactor_impl = 0) : reactor_impl_(reactor_impl){}
    virtual ~reactor(){}
    using Event_Type = event_handler::Event_Type;
    virtual void register_handler(event_handler* handler, Event_Type type){
        reactor_impl_->register_handler(handler, type);
    }
    virtual void register_handler(int handle, event_handler *handler, Event_Type type) {
        reactor_impl_->register_handler(handle, handler, type);
    }
    virtual void remove_handler(event_handler *handler, Event_Type type) {
        reactor_impl_->remove_handler(handler, type);
    }
    virtual void remove_handler(int handle, event_handler *handler, Event_Type type) {
        reactor_impl_->remove_handler(handle, handler, type);
    }
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

//TODO seperate these classes to another file
class reactor_implementation{
public:
    using Event_Type = event_handler::Event_Type;
    reactor_implementation() = default;
    virtual ~reactor_implementation(){}

    virtual void handle_events(std::chrono::microseconds *timeout);
    virtual void register_handler(event_handler* handler, Event_Type type) = 0;
    virtual void register_handler(int handle, event_handler *handler, Event_Type type) = 0;
    virtual void remove_handler(event_handler *handler, Event_Type type) = 0;
    virtual void remove_handler(int handle, event_handler *handler, Event_Type type) = 0;
private:
    
};

class select_reactor_impl : public reactor_implementation{
public:
    void handle_events(std::chrono::microseconds *timeout) override{

    }
    void register_handler(event_handler* handler, Event_Type type) override{

    }
    void register_handler(int handle, event_handler *handler, Event_Type type) override{

    }
    void remove_handler(event_handler *handler, Event_Type type) override{

    }
    void remove_handler(int handle, event_handler *handler, Event_Type type) override{

    }
private:
    demultiplex_table demux_table_;
};

class poll_reactor_iml : public reactor_implementation{
public:
    void handle_events(std::chrono::microseconds *timeout) override{

    }
    void register_handler(event_handler* handler, Event_Type type) override{

    }
    void register_handler(int handle, event_handler *handler, Event_Type type) override{

    }
    void remove_handler(event_handler *handler, Event_Type type) override{

    }
    void remove_handler(int handle, event_handler *handler, Event_Type type) override{

    }
private:
    demultiplex_table demux_table_;
};
} // reactor



#endif // _UNP_REACTOR_H_
