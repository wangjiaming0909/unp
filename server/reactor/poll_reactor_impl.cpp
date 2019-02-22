#include "server/reactor/poll_reactor_impl.h"

poll_reactor_impl::poll_reactor_impl() : wait_pfds_(), demux_table_()
{

}

poll_reactor_impl::~poll_demultiplex_table(){}

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