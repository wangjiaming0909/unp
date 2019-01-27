#ifndef _UNP_REACTOR_CONNECTOR_H_
#define _UNP_REACTOR_CONNECTOR_H_

#include "server/reactor/event_handler.h"
#include "server/net/sock_connector.h"
#include "server/thread/message_queue.h"
#include "server/reactor/io_handler.h"
#include "server/thread/thread_pool.h"
#include "boost/shared_ptr.hpp"
#include <vector>

namespace reactor {

template <typename data_type>
class connector : public event_handler {
public:
    using io_handler_type = reactor::io_handler<data_type>;
    using message_queue_type = thread::message_queue<data_type>;
    connector(Reactor& react);
    virtual ~connector();
    //got messages to read
    virtual int handle_input(int handle) override ;
    // virtual int handle_output(int handle) override;
    virtual int handle_timeout(int handle) override ;
    //close write
    virtual int handle_close(int handle) override ;
    virtual int handle_signal(int handle) override ;
    // virtual int get_handle() const {}
    int connect(const net::inet_addr& target_addr);
    int connect_n(const net::inet_addr& target_addr) ;
    
private:
    int open();
    int make_io_handler();
    int activate_io_handler();
    int connect_i(const net::inet_addr& target_addr);
private:
    thread::thread_pool                                 &pool_;
    message_queue_type                                  *mq_;
    net::sock_connector                                 connector_;
    std::vector<boost::shared_ptr<io_handler_type>>     io_handlers_;
};

template <typename data_type>
connector<data_type>::connector(Reactor& react) : event_handler(react){ }

template <typename data_type>
connector<data_type>::~connector(){}

//connect succeed
template <typename data_type>
int connector<data_type>::handle_input(int handle) {}

template <typename data_type>
int connector<data_type>::handle_timeout(int handle) {}

template <typename data_type>
int connector<data_type>::handle_close(int handle) {}

template <typename data_type>
int connector<data_type>::handle_signal(int handle) {}

template <typename data_type>
int connector<data_type>::connect(const net::inet_addr& target_addr){
    connect_i(target_addr);
}

template <typename data_type>
int connector<data_type>::connect_n(const net::inet_addr& target_addr){}

template <typename data_type>
int connector<data_type>::make_io_handler(){ 

}

template <typename data_type>
int connector<data_type>::activate_io_handler(){}

template <typename data_type>
int connector<data_type>::connect_i(const net::inet_addr& target_addr){
    make_io_handler();
    activate_io_handler();
}
} // reactor

#endif // _UNP_REACTOR_CONNECTOR_H_

