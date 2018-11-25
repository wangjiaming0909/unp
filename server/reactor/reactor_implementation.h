#ifndef _UNP_REACTOR_IMPLEMENTATION_H_
#define _UNP_REACTOR_IMPLEMENTATION_H_

#include "event_handler.h"
#include <chrono>
namespace reactor{

class reactor_implementation{
public:
    using Event_Type = event_handler::Event_Type;
    reactor_implementation() = default;
    virtual ~reactor_implementation(){}
    virtual void handle_events(std::chrono::microseconds *timeout) = 0;
    virtual void register_handler(event_handler* handler, Event_Type type) = 0;
    // virtual void register_handler(int handle, event_handler *handler, Event_Type type) = 0;
    virtual void remove_handler(event_handler *handler, Event_Type type) = 0;
    // virtual void remove_handler(int handle, event_handler *handler, Event_Type type) = 0;
};

}
#endif //_UNP_REACTOR_IMPLEMENTATION_H_
