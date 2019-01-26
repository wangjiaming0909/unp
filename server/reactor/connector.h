#ifndef _UNP_REACTOR_CONNECTOR_H_
#define _UNP_REACTOR_CONNECTOR_H_

#include "server/reactor/event_handler.h"
#include "server/net/sock_connector.h"

namespace reactor {

class connector : public event_handler
{
public:
    connector(Reactor& react);
    virtual ~connector();
private:
    net::sock_connector connector_;
};

connector::connector(Reactor& react) : event_handler(react) { }

connector::~connector() { }

} // reactor

#endif // _UNP_REACTOR_CONNECTOR_H_

