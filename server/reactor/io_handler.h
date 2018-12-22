#ifndef _UNP_IO_HANDLER_H_
#define _UNP_IO_HANDLER_H_

#include "reactor.h"
#include "event_handler.h"

namespace reactor{

//1, io_handler is an event_handler
//2, io_handler is used to handle sock_stream events
    //like read/write from/to sock_stream
    //io_handler will be created by acceptor
//3, io_handler should be a task ==> means that io_handler should inheret from class task
    //means that io_handler will handle the event in a new thread
class io_handler : public event_handler{
public:
    io_handler(Reactor* react);
    
    virtual int handle_input(int handle);
    virtual int handle_output(int handle);
    virtual int handle_timeout(int handle);
    virtual int handle_close(int handle);
    virtual int handle_signal(int handle);
    virtual int get_handle() const;
    virtual void set_handle(int handle);
    
};
}
#endif // _UNP_IO_HANDLER_H_