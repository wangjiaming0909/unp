#ifndef _UNP_REACTOR_H_
#define _UNP_REACTOR_H_

#include <chrono>
#include <event_handler.h>

namespace reactor
{
class event_handler;
class reactor_implementation;
class reactor{
public:
    using Event_Type = event_handler::Event_Type;
    virtual void register_handler(event_handler* handler, Event_Type type) = 0;
    virtual void register_handler(int handle, event_handler *handler, Event_Type type) = 0;
    virtual void remove_handler(event_handler *handler, Event_Type type) = 0;
    virtual void remove_handler(int handle, event_handler *handler, Event_Type type) = 0;
    void handle_events(std::chrono::microseconds *timeout = 0);
    static reactor* instance();
private:
    reactor_implementation *reactor_impl;
};
} // reactor



#endif // _UNP_REACTOR_H_