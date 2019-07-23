#ifndef ECHO_CONNECTION_HANDLER_H_
#define ECHO_CONNECTION_HANDLER_H_
#include "reactor/connection_handler.h"

namespace reactor
{

class echo_connection_handler : public connection_handler
{
public:
    echo_connection_handler(Reactor& reactor);
    virtual ~echo_connection_handler();
    virtual int handle_input(int handle) override;
private:
    // static int i;
};
    
}
#endif