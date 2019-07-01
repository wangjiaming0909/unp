#ifndef _UNP_REACTOR_TIMEOUT_HANDLER_H_
#define _UNP_REACTOR_TIMEOUT_HANDLER_H_

#include "reactor/event_handler.h"

namespace reactor
{
class TimeoutHandler : public event_handler
{
public:
    TimeoutHandler(Reactor& reactor);
    virtual ~TimeoutHandler();
private:
    /* data */
};

}
#endif //_UNP_REACTOR_TIMEOUT_HANDLER_H_