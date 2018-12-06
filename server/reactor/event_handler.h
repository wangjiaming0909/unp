#ifndef _UNP_REACTOR_EVENT_HANDLER_H_
#define _UNP_REACTOR_EVENT_HANDLER_H_

#include "../../server/util/easylogging++.h"
#include <poll.h>
#include <unistd.h>
#include "../net/macros.h"

namespace reactor
{
class reactor;

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
        NONE = 0x001,
        READ_EVENT = POLLIN,
        EXCEPT_EVENT = POLLPRI,//0x2
        WRITE_EVENT = POLLOUT,//0x4
        ACCEPT_EVENT = 0x008,
        TIMEOUT_EVENT = 0x010,
        SIGNAL_EVENT = 0x020,
        CLOSE_EVENT = 0x040,
    };
    event_handler(reactor* react) : reactor_(react) {}
    //these functions can't be pure virtual
    //because some handlers may not need to implement all of them
    //so the these handlers can be non-abstract classes
    virtual int handle_input(int handle){}
    virtual int handle_output(int handle){}
    virtual int handle_timeout(int handle){}
    virtual int handle_close(int handle){}
    virtual int handle_signal(int handle){}
    virtual int get_handle() const{}
    virtual void set_handle(int handle){}
protected:
    virtual ~event_handler(){}
    reactor* reactor_;
};

class default_event_handler : public event_handler{
public:
    default_event_handler(reactor* react)
        : event_handler(react)
    {}
    ~default_event_handler(){}
    virtual int handle_input(int handle) override {
        char buffer[32] = {};
        int reads = ::read(handle, buffer, 32);
        LOG(INFO) << "handle_input handle: " << handle << ", " << reads << " bytes read: " << buffer;
    }
    virtual int handle_output(int handle) override {
        LOG(INFO) << "handle_output: " << handle << "...";
    }
    virtual int handle_timeout(int handle) override {
        LOG(INFO) << "handle_timeout: " << handle << "...";
    }
    virtual int handle_close(int handle) override {
        LOG(INFO) << "handle_close: " << handle << "...";
    }
    virtual int handle_signal(int handle) override {
        LOG(INFO) << "handle_signal: " << handle << "...";
    }
};
} // reactor



#endif // _UNP_REACTOR_EVENT_HANDLER_H_
