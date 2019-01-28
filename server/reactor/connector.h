#ifndef _UNP_REACTOR_CONNECTOR_H_
#define _UNP_REACTOR_CONNECTOR_H_

#include "server/reactor/event_handler.h"
#include "server/net/sock_connector.h"
#include "server/thread/message_queue.h"
#include "server/reactor/io_handler.h"
#include "server/thread/thread_pool.h"
#include "server/util/easylogging++.h"
#include "boost/shared_ptr.hpp"
#include <vector>
#include <chrono>

namespace reactor {

template <typename DataType, typename Handler>
class reactor_connector : public event_handler {
public:
    using IOHandlerType = Handler;
    using MessageQueueType = thread::message_queue<DataType>;
    using micro_seconds = std::chrono::microseconds;

    reactor_connector(Reactor& react, thread::thread_pool& pool, MessageQueueType& mq);
    virtual ~reactor_connector() override;
    //got messages to read
    virtual int handle_input(int handle) override ;
    // virtual int handle_output(int handle) override;
    virtual int handle_timeout(int handle) override ;
    //close write
    virtual int handle_close(int handle) override ;
    virtual int handle_signal(int handle) override ;
    // virtual int get_handle() const {}
    int connect(const net::inet_addr& target_addr, const micro_seconds& timeout);
    int connect_n(const net::inet_addr& target_addr, int n, const micro_seconds& timeout) ;
    
private:
    using IOHandlerPtr = boost::shared_ptr<IOHandlerType>;

private:
    int open();
    int make_handler();
    int activate_io_handler(int handle);
    int connect_i(const net::inet_addr& target_addr, const micro_seconds& timeout);
private:
    thread::thread_pool                                 *pool_;
    MessageQueueType                                    *mq_;
    net::sock_connector                                 connector_;
    std::vector<IOHandlerPtr>                           handlers_;
};

template <typename DataType, typename Handler>
reactor_connector<DataType, Handler>::reactor_connector( Reactor& react
    , thread::thread_pool& pool
    , MessageQueueType& mq) 
    : event_handler(react)
    , pool_(&pool)
    , mq_(&mq)
    , connector_()
    , handlers_()
{
    this->open();
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::open(){}

template <typename DataType, typename Handler>
reactor_connector<DataType, Handler>::~reactor_connector(){}

//connect succeed
template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::handle_input(int handle) {}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::handle_timeout(int handle) {}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::handle_close(int handle) {}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::handle_signal(int handle) {}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::connect(
    const net::inet_addr& target_addr, 
    const micro_seconds& timeout)
{
    return connect_i(target_addr, timeout);
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::connect_n(
    const net::inet_addr& target_addr, 
    int n, const micro_seconds& timeout)
{

}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::make_handler()
{ 
    auto* handler = new Handler(reactor_, *pool_, *mq_);
    handlers_[handler->get_handle()].reset(handler);
    return handler->get_handle();
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::activate_io_handler(int handle)
{
    //set non-blocking
    if(handlers_[handle]->get_sock_stream().get_sock_fd().set_non_blocking() != 0){
        LOG(WARNING) << "handle: " << handle << " setting non-blocking error" << strerror(errno);
        handlers_[handle]->close();
        handlers_[handle].reset(0);
        return -1;
    }

    //open handler
    if(handlers_[handle]->open() != 0){
        LOG(WARNING) << "handle: " << handle << " open error" << strerror(errno);
        handlers_[handle]->close();
        handlers_[handle].reset(0);
        return -1;
    }
    return 0;
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::connect_i(const net::inet_addr& target_addr, const micro_seconds& timeout)
{
    //make handler
    int handle = make_handler();
    
    //connect
    if(connector_.connect(handlers_[handle]->get_sock_stream(), target_addr, timeout) != 0){
        LOG(WARNING) << "connect to " << target_addr.get_address_string() << " error...";
        handlers_[handle]->close();
        handlers_[handle].reset(0);
        return -1;
    }

    //activate handler: bind to reactor
    return activate_io_handler(handle);
}
} // reactor

#endif // _UNP_REACTOR_CONNECTOR_H_

