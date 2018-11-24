#ifndef _UNP_POLL_REACTOR_IMPL_H_
#define _UNP_POLL_REACTOR_IMPL_H_

#include "reactor_implementation.h"
#include "demultiplex_table.h"
namespace reactor{

class poll_reactor_iml : public reactor_implementation{
public:
    void handle_events(std::chrono::microseconds *timeout) override{

    }
    void register_handler(event_handler* handler, Event_Type type) override{

    }
    // void register_handler(int handle, event_handler *handler, Event_Type type) override{ }
    void remove_handler(event_handler *handler, Event_Type type) override{

    }
    // void remove_handler(int handle, event_handler *handler, Event_Type type) override{ }
private:
    // demultiplex_table demux_table_;
};



}
#endif //_UNP_POLL_REACTOR_IMPL_H_
