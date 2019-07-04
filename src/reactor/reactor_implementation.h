#ifndef _UNP_REACTOR_IMPLEMENTATION_H_
#define _UNP_REACTOR_IMPLEMENTATION_H_

#include "reactor/EventHandler.h"
#include <chrono>
#include <mutex>
namespace reactor{

class reactor_implementation{
public:
    using Event_Type = EventHandler::Event_Type;
    reactor_implementation() = default;
    virtual ~reactor_implementation(){}
    virtual int handle_events(std::chrono::microseconds *timeout) = 0;
    virtual int register_handler(EventHandler* handler, Event_Type type) = 0;
    virtual int register_handler(int handle, EventHandler *handler, Event_Type type) = 0;
    virtual int unregister_handler(EventHandler *handler, Event_Type type) = 0;
    virtual int unregister_handler(int handle, EventHandler *handler, Event_Type type) = 0;
    bool isWaiting() const {return isWaiting_;}
protected:
    bool        isWaiting_ = false;
};

static const int USING_EPOLL = 1;
static const int USING_POLL = 0;

short reactor_event_to_poll_event(EventHandler::Event_Type type, int poll_or_epoll);

} // namespace reactor
#endif //_UNP_REACTOR_IMPLEMENTATION_H_
