#ifndef _UNP_REACTOR_EventHandler_H_
#define _UNP_REACTOR_EventHandler_H_

#include <poll.h>
#include <unistd.h>

#include "util/easylogging++.h"
#include "net/macros.h"
#include "util/XString.h"
#include "boost/noncopyable.hpp"

namespace reactor
{
class Reactor;

//class EventHandler
/*
    1, EventHandler do not has handle member in it
    2, EventHandler need a reactor pointer as a parameter of the constructor, and a member of reactor*
    3, all the handle_* function can't be pure virtual
*/
class EventHandler{
public:
    friend class Reactor;
    typedef unsigned int Event_Type;
    enum{
        NONE            = 0x000,
        READ_EVENT      = POLLIN,
        EXCEPT_EVENT    = POLLPRI,//0x2
        WRITE_EVENT     = POLLOUT,//0x4
        ACCEPT_EVENT    = 1 << 3,
        TIMEOUT_EVENT   = 1 << 4,
        SIGNAL_EVENT    = 1 << 5,
        CLOSE_EVENT     = 1 << 6,
        CONNECT_EVENT   = 1 << 7
    };
    EventHandler() : reactor_(nullptr), isRegistered_(false){}
    EventHandler(Reactor& react) : reactor_(&react) {}
    //these functions can't be pure virtual
    //because some handlers may not need to implement all of them
    //so the these handlers can be non-abstract classes
    //** why give default implementation?
    //** cause some subclasses will not implement some of them(and do not need them)
    //** if do not give them default implementation,
    //** subclasses have to implement some functions that they do not need 
    virtual int handle_input(int ){ return 0; }
    virtual int handle_output(int ){ return 0; }
    virtual int handle_timeout(int ) noexcept { return 0; }
    virtual int handle_close(int ){ return 0; }
    virtual int close_read(int){ return 0; }
    virtual int close_write(int){ return 0; }
    virtual int handle_signal(int ){ return 0; }
    virtual int get_handle() const{ return 0; }
    virtual void set_handle(int ){}
    Reactor* get_reactor() const {return reactor_;}
    void setReactor(Reactor& reactor) {reactor_ = &reactor;}
    bool isAttachedToReactor() const {return reactor_ != nullptr;}
    bool isRegistered() const {return isRegistered_;}

protected:
    virtual ~EventHandler(){}
    Reactor* reactor_;
    bool isRegistered_;
};

inline util::string event_type_to_string(EventHandler::Event_Type type){
    switch(type){
        case EventHandler::NONE:
            return util::string("NONE");
        case EventHandler::READ_EVENT:
            return util::string("READ_EVENT");
        case EventHandler::SIGNAL_EVENT:
            return util::string("SIGNAL_EVENT");
        case EventHandler::TIMEOUT_EVENT:
            return util::string("TIMEOUT_EVENT");
        case EventHandler::WRITE_EVENT:
            return util::string("WRITE_EVENT");
        case EventHandler::ACCEPT_EVENT:
            return util::string("ACCEPT_EVENT");
        case EventHandler::CLOSE_EVENT:
            return util::string("CLOSE_EVENT");
        default:
            return util::string();
    }
}

class default_EventHandler : public EventHandler{
public:
    default_EventHandler(Reactor& react) : EventHandler(react) {}
    ~default_EventHandler(){}
    virtual int handle_input(int handle) override {
        char buffer[32] = {};
        int reads = ::read(handle, buffer, 32);
        LOG(INFO) << "handle_input handle: " << handle << ", " << reads << " bytes read: " << buffer;
        return 0;
    }
    virtual int handle_output(int handle) override {
        LOG(INFO) << "handle_output: " << handle << "...";
        return 0;
    }
    virtual int handle_timeout(int handle) noexcept override {
        LOG(INFO) << "handle_timeout: " << handle << "...";
        return 0;
    }
    virtual int handle_close(int handle) override {
        LOG(INFO) << "handle_close: " << handle << "...";
        return 0;
    }
    virtual int handle_signal(int handle) override {
        LOG(INFO) << "handle_signal: " << handle << "...";
        return 0;
    }
};
} // reactor



#endif // _UNP_REACTOR_EventHandler_H_
