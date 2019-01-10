#ifndef _UNP_REACTOR_EVENT_HANDLER_H_
#define _UNP_REACTOR_EVENT_HANDLER_H_

#include "server/util/easylogging++.h"
#include <poll.h>
#include <unistd.h>
#include "server/net/macros.h"
#include "server/util/XString.h"

namespace reactor
{
class Reactor;

//class event_handler
/*
    1, event_handler do not has handle member in it
    2, event_handler need a reactor pointer as a parameter of the constructor, and a member of reactor*
    3, all the handle_* function can't be pure virtual
*/
class event_handler{
public:
    typedef unsigned int Event_Type;
    enum{
        NONE = 0x000,
        READ_EVENT = POLLIN,
        EXCEPT_EVENT = POLLPRI,//0x2
        WRITE_EVENT = POLLOUT,//0x4
        ACCEPT_EVENT = 0x008,
        TIMEOUT_EVENT = 0x010,
        SIGNAL_EVENT = 0x020,
        CLOSE_EVENT = 0x040,
    };
    event_handler(Reactor& react) : reactor_(&react) {}
    //these functions can't be pure virtual
    //because some handlers may not need to implement all of them
    //so the these handlers can be non-abstract classes
    virtual int handle_input(int ){ return 0; }
    virtual int handle_output(int ){ return 0; }
    virtual int handle_timeout(int ){ return 0; }
    virtual int handle_close(int ){ return 0; }
    virtual int handle_signal(int ){ return 0; }
    virtual int get_handle() const{ return 0; }
    virtual void set_handle(int ){}
    virtual Reactor* get_reactor() const {return reactor_;}
protected:
    virtual ~event_handler(){}
    Reactor* reactor_;
};

inline util::string event_type_to_string(event_handler::Event_Type type){
    switch(type){
        case event_handler::NONE:
            return util::string("NONE");
        case event_handler::READ_EVENT:
            return util::string("READ_EVENT");
        case event_handler::SIGNAL_EVENT:
            return util::string("SIGNAL_EVENT");
        case event_handler::TIMEOUT_EVENT:
            return util::string("TIMEOUT_EVENT");
        case event_handler::WRITE_EVENT:
            return util::string("WRITE_EVENT");
        case event_handler::ACCEPT_EVENT:
            return util::string("ACCEPT_EVENT");
        case event_handler::CLOSE_EVENT:
            return util::string("CLOSE_EVENT");
        default:
            return util::string();
    }
}

class default_event_handler : public event_handler{
public:
    default_event_handler(Reactor& react) : event_handler(react) {}
    ~default_event_handler(){}
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
    virtual int handle_timeout(int handle) override {
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



#endif // _UNP_REACTOR_EVENT_HANDLER_H_
