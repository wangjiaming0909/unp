#include "server/reactor/poll_reactor_impl.h"

poll_reactor_impl::poll_reactor_impl() : wait_pfds_(), demux_table_()
{

}

poll_reactor_impl::~poll_reactor_impl(){}

void poll_reactor_impl::handle_events(std::chrono::microseconds *timeout)
{

}

int poll_reactor_impl::register_handler(event_handler* handler, Event_Type type) 
{ 
    (void)handler;
    (void)type;
}
int poll_reactor_impl::unregister_handler(event_handler *handler, Event_Type type) 
{ 
    (void)handler;
    (void)type;
}

int  poll_reactor_impl::register_handler(int handle, event_handler *handler, Event_Type type)
{
    if(handle == INVALID_HANDLE || handler == 0 || type == event_handler::NONE){
        LOG(ERROR) << "handle error or registered type error...";
        return -1;
    }
    //already existed in the table
    if(demux_table_.get_handler(handle, type) != 0)
        LOG(WARNING) << "Already existed in the demultiplex table, handle: " 
        << handle << " event: " << event_type_to_string(type);

    // wait_pfds_.push_back(new )

}


int poll_reactor_impl::unregister_handler(int handle, event_handler *handler, Event_Type type)
{

}



epoll_reactor_impl::epoll_reactor_impl()
{

}

epoll_reactor_impl::~epoll_reactor_impl()
{

}

void epoll_reactor_impl::handle_events(std::chrono::microseconds *timeout)
{

}

int epoll_reactor_impl::register_handler(event_handler* handler, Event_Type type)
{

}

int epoll_reactor_impl::register_handler(int handle, event_handler *handler, Event_Type type)
{

}

int epoll_reactor_impl::unregister_handler(event_handler *handler, Event_Type type)
{

}

int epoll_reactor_impl::unregister_handler(int handle, event_handler *handler, Event_Type type)
{

}