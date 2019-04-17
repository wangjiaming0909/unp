#ifndef _UNP_REACTOR_CONNECTOR_H_
#define _UNP_REACTOR_CONNECTOR_H_

#include "server/reactor/event_handler.h"
#include "server/net/sock_connector.h"
#include "server/thread/message_queue.h"
#include "server/reactor/io_handler.h"
#include "server/thread/thread_pool.h"
#include "server/util/easylogging++.h"
#include "boost/shared_ptr.hpp"
#include "connection_handler.h"
#include <vector>
#include <chrono>
#include <map>
#include <hash_map>

namespace reactor {


class connector : public event_handler{
public:
    using micro_seconds = std::chrono::microseconds;
	using connection_handler_ptr_t = std::shared_ptr<connection_handler>;
	using map_t = std::unordered_map<int, connection_handler_ptr_t>;
	connector(Reactor& react) 
		: event_handler(react)
		, connector_()
		, handlers_()
		, connection_handlers_()
	{

	}
	virtual ~connector() override;
    virtual int handle_input(int handle) override ;
	
    int connect(const net::inet_addr& target_addr, const micro_seconds& timeout);
private:
    int make_connection_handler();
    int activate_connection_handler(int handle);
	
private:
	net::sock_connector 						connector_;
	std::vector<connection_handler_ptr_t> 		handlers_;
	//key is the handle
	map_t 										connection_handlers_;
};

template <typename DataType, typename Handler>
class reactor_connector : public event_handler {
public:
    using IOHandlerType = Handler;
    using MessageQueueType = thread::message_queue<DataType>;
    using micro_seconds = std::chrono::microseconds;
private:
    using IOHandlerPtr = boost::shared_ptr<IOHandlerType>;

public:
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

    const std::vector<IOHandlerPtr>* get_handlers() const {return handlers_;}
    
private:
    int open();
	int close();
    Handler* make_handler();
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
int reactor_connector<DataType, Handler>::close()
{

}

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
Handler* reactor_connector<DataType, Handler>::make_handler()
{ 
    //handler sock_stream
    auto* handler = new Handler(*reactor_, *pool_, *mq_);
    if(handler == 0) return nullptr;

    // handlers_[handle].reset(handler);
    // return handler->get_handle();
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::activate_io_handler(int handle)
{
    //set non-blocking
    if(handlers_[handle]->get_sock_stream().get_sock_fd().set_non_blocking() != 0){
        LOG(WARNING) << "handle: " << handle << " setting non-blocking error" << strerror(errno);
        handlers_[handle]->close();
        handlers_[handle].reset();
        return -1;
    }

    //open handler
    if(handlers_[handle]->open() != 0){
        LOG(WARNING) << "handle: " << handle << " open error" << strerror(errno);
        handlers_[handle]->close();
        handlers_[handle].reset();
        return -1;
    }
    return 0;
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::connect_i(const net::inet_addr& target_addr, const micro_seconds& timeout)
{
    //make handler
    auto* newHandler = make_handler();
    
    auto &stream = newHandler->get_sock_stream();
    //connect will set the stream's fd
    if(connector_.connect(stream, target_addr, &timeout, 1, 0) != 0){
        LOG(WARNING) << "connect to " << target_addr.get_address_string() << " error...";
        newHandler->close();
        return -1;
    }

    //record the handle in handlers_
    int handle = stream.get_handle();
    if(handlers_.size() < handle) handlers_.resize(handle * 2);
    handlers_[handle].reset(newHandler);

    //activate handler: bind to reactor
    return activate_io_handler(handle);
}
} // reactor

#endif // _UNP_REACTOR_CONNECTOR_H_

