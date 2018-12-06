#ifndef _UNP_IO_HANDLER_H_
#define _UNP_IO_HANDLER_H_

#include "reactor.h"
#include "event_handler.h"

namespace reactor{

class io_handler : public event_handler{
public:
    io_handler(reactor* react) : event_handler(react){}
    
    virtual int handle_input(int handle){}
    virtual int handle_output(int handle){}
    virtual int handle_timeout(int handle){}
    virtual int handle_close(int handle){}
    virtual int handle_signal(int handle){}
    virtual int get_handle() const{}
    virtual void set_handle(int handle){}
private:
    
};
}
#endif // _UNP_IO_HANDLER_H_