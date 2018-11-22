#ifndef _UNP_SELECT_REACTOR_IMPL_H_
#define _UNP_SELECT_REACTOR_IMPL_H_

#include "reactor_implementation.h"
namespace reactor{

//event tuple
struct select_event_tuple{
    using Event_Type = event_handler::Event_Type;
    //event_handler
    event_handler  *event_handler_;
    //event_type
    Event_Type event_type_;
    //handle
    int handle_;
};

class demultiplex_table{
public:
//    using Event_Type = event_handler::Event_Type;
    select_event_tuple table_[sizeof(unsigned long int)];
};

class select_reactor_impl : public reactor_implementation{
public:
    void handle_events(std::chrono::microseconds *timeout) override{

    }
    void register_handler(event_handler* handler, Event_Type type) override{

    }
    void register_handler(int handle, event_handler *handler, Event_Type type) override{

    }
    void remove_handler(event_handler *handler, Event_Type type) override{

    }
    void remove_handler(int handle, event_handler *handler, Event_Type type) override{

    }
private:
    demultiplex_table demux_table_;
};


}

#endif //_UNP_SELECT_REACTOR_IMPL_H_
