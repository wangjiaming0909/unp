#ifndef _UNP_REACTOR_EVENT_HANDLER_H_
#define _UNP_REACTOR_EVENT_HANDLER_H_

#include "../../server/util/easylogging++.h"

namespace reactor
{
class event_handler{
public:
    typedef unsigned int Event_Type;
    enum{
        NONE = 01,
        READ_EVENT = 01,
        ACCEPT_EVENT = 02,
        WRITE_EVENT = 03,
        TIMEOUT_EVENT = 04,
        SIGNAL_EVENT = 010,
        CLOSE_EVENT = 020
    };
    event_handler(int handle) : handle_(handle){};
    virtual void handle_input(int handle) = 0;
    virtual void handle_output(int handle) = 0;
    virtual void handle_timeout(int handle) = 0;
    virtual void handle_close(int handle) = 0;
    virtual void handle_signal(int handle) = 0;
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
    virtual void handle_input(int handle) override {
        LOG(INFO) << "handle_input handle: " << handle << "...";
    }
    virtual void handle_output(int handle) override {
        LOG(INFO) << "handle_output: " << handle << "...";
    }
    virtual void handle_timeout(int handle) override {
        LOG(INFO) << "handle_timeout: " << handle << "...";
    }
    virtual void handle_close(int handle) override {
        LOG(INFO) << "handle_close: " << handle << "...";
    }
    virtual void handle_signal(int handle) override {
        LOG(INFO) << "handle_signal: " << handle << "...";
    }
};
} // reactor



#endif // _UNP_REACTOR_EVENT_HANDLER_H_