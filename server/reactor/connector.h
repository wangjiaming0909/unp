#ifndef _UNP_REACTOR_CONNECTOR_H_
#define _UNP_REACTOR_CONNECTOR_H_

#include "server/reactor/event_handler.h"
#include "server/net/sock_connector.h"

namespace reactor {

class connector : public event_handler {
public:
    connector(Reactor& react)
        : event_handler(react){}
    virtual ~connector(){}
    //got messages to read
    virtual int handle_input(int handle) override {}
    // virtual int handle_output(int handle) override;
    virtual int handle_timeout(int handle) override {}
    //close write
    virtual int handle_close(int handle) override {}
    virtual int handle_signal(int handle) override {}
    // virtual int get_handle() const {}
    int connect();
    
private:
    int activate_io_handler(){}
    int connect(const net::inet_addr& target_addr){}
    int connect_n(const net::inet_addr& target_addr){}
private:
    net::sock_connector         connector_;
};
} // reactor

#endif // _UNP_REACTOR_CONNECTOR_H_

