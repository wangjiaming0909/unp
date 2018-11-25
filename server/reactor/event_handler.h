#ifndef _UNP_REACTOR_EVENT_HANDLER_H_
#define _UNP_REACTOR_EVENT_HANDLER_H_

#include "../../server/util/easylogging++.h"
#include <poll.h>

namespace reactor
{
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
    event_handler(int handle) : handle_(handle){};
    virtual int handle_input(int handle) = 0;
    virtual int handle_output(int handle) = 0;
    virtual int handle_timeout(int handle) = 0;
    virtual int handle_close(int handle) = 0;
    virtual int handle_signal(int handle) = 0;
    virtual int get_handle() const {return handle_;}
protected:
    virtual ~event_handler(){}
private:
    int handle_;
};


class default_event_handler : public event_handler{
public:
    default_event_handler(int handle) : event_handler(handle){}
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