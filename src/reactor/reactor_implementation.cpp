#include "reactor/reactor_implementation.h"
#include <poll.h>
#include <sys/epoll.h>

namespace reactor
{

short reactor_event_to_poll_event(EventHandler::Event_Type type, int poll_or_epoll )
{
    short events = 0;

    if(type == EventHandler::READ_EVENT || 
        type == EventHandler::ACCEPT_EVENT || 
        type == EventHandler::CONNECT_EVENT){
        if(poll_or_epoll == USING_POLL) 
            events |= POLLIN;
        else 
            events |= EPOLLIN;
    }
    if(type == EventHandler::WRITE_EVENT || 
        type == EventHandler::CONNECT_EVENT){
        if(poll_or_epoll == USING_POLL) 
            events |= POLLOUT;
        else 
            events |= EPOLLOUT;
    }
    return events;
        // NONE            = 0x000,
        // READ_EVENT      = POLLIN,
        // EXCEPT_EVENT    = POLLPRI,//0x2
        // WRITE_EVENT     = POLLOUT,//0x4
        // ACCEPT_EVENT    = 1 << 3,
        // TIMEOUT_EVENT   = 1 << 4,
        // SIGNAL_EVENT    = 1 << 5,
        // CLOSE_EVENT     = 1 << 6,
        // CONNECT_EVENT   = 1 << 7
}

}//namespace reactor