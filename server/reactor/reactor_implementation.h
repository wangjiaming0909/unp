#ifndef _UNP_REACTOR_IMPLEMENTATION_H_
#define _UNP_REACTOR_IMPLEMENTATION_H_

#include "server/reactor/event_handler.h"
#include <chrono>
#include <mutex>
namespace reactor{

class reactor_implementation{
public:
    using Event_Type = event_handler::Event_Type;
    using mutex_t = std::mutex;
    reactor_implementation() = default;
    virtual ~reactor_implementation(){}
    virtual int handle_events(std::chrono::microseconds *timeout) = 0;
    virtual int register_handler(event_handler* handler, Event_Type type) = 0;
    virtual int register_handler(int handle, event_handler *handler, Event_Type type) = 0;
    virtual int unregister_handler(event_handler *handler, Event_Type type) = 0;
    virtual int unregister_handler(int handle, event_handler *handler, Event_Type type) = 0;
    bool isWaiting() const {return isWaiting_;}
protected:
    bool        isWaiting_ = false;
    mutex_t     mutex_;
};
}
#endif //_UNP_REACTOR_IMPLEMENTATION_H_
