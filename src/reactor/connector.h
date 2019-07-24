#ifndef _UNP_REACTOR_CONNECTOR_H_
#define _UNP_REACTOR_CONNECTOR_H_

#include "reactor/EventHandler.h"
#include "net/sock_connector.h"
#include "thread/message_queue.h"
#include "reactor/io_handler.h"
#include "thread/thread_pool.h"
#include "util/easylogging++.h"
#include "reactor/connection_handler.h"
#include "boost/intrusive/list_hook.hpp"
#include "net/unp.h"

#include <vector>
#include <chrono>
#include <map>
#include <unordered_map>

namespace reactor
{

using IntrusiveListBaseHook = boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;
struct ServiceT : public IntrusiveListBaseHook
{
    virtual ~ServiceT() = default;
    virtual int close() = 0;
};


template <typename Handler_t>
struct IConnector : public ServiceT
{
    using micro_seconds = std::chrono::microseconds;
    using HandlerT = Handler_t  ;
    using ConnectionHandlerPtr_t = Handler_t*;

    IConnector(ConnectionHandlerPtr_t handlerP) : connector_{}, handlerPtr_{handlerP}{}
    virtual Handler_t* connect(const net::inet_addr& targetAddr, micro_seconds timeout) = 0;
    virtual int disconnect(micro_seconds timeout = std::chrono::microseconds{1}) = 0;
    int close() override { disconnect(); }
    virtual ~IConnector() = default;
TEST_PROTECTED:
    net::sock_connector connector_;
    ConnectionHandlerPtr_t handlerPtr_;
};

template <typename Handler_t >
class connector : public IConnector<Handler_t>
{
public:
    using Base_t = IConnector<Handler_t>;
    using micro_seconds = typename Base_t::micro_seconds;
    using HandlerT = typename Base_t::HandlerT;
    using ConnectionHandlerPtr_t = typename Base_t::ConnectionHandlerPtr_t;

    connector(Handler_t& handler) : Base_t(&handler) { }
    virtual ~connector() override { delete this->handlerPtr_;}

    virtual ConnectionHandlerPtr_t connect(const net::inet_addr &target_addr, micro_seconds timeout) override;
    virtual int disconnect(micro_seconds timeout) override;
};

template <typename Handler_t>
typename connector<Handler_t>::ConnectionHandlerPtr_t connector<Handler_t>::connect(const net::inet_addr& target_addr, micro_seconds timeout)
{
    net::sock_stream &stream = this->handlerPtr_->get_sock_stream();

    if (this->connector_.connect(stream, target_addr, &timeout, 1, 0) != 0)
    {
        LOG(WARNING) << "connect to " << target_addr.get_address_string() << " error...";
        return nullptr;
   }

   if(this->handlerPtr_->open() < 0)
    {
        LOG(WARNING) << "activate connection handler error";
        return nullptr;
    }
    return this->handlerPtr_;
}

template <typename Handler_t>
int connector<Handler_t>::disconnect(micro_seconds timeout)
{
    this->handlerPtr_->close();
}


template <typename DataType, typename Handler>
class reactor_connector : public EventHandler
{
public:
    using IOHandlerType = Handler;
    using MessageQueueType = thread::message_queue<DataType>;
    using micro_seconds = std::chrono::microseconds;

private:
    using IOHandlerPtr = std::shared_ptr<IOHandlerType>;

public:
    reactor_connector(Reactor &react, thread::thread_pool &pool, MessageQueueType &mq);
    virtual ~reactor_connector() override;
    //got messages to read
    virtual int handle_input(int handle) override;
    // virtual int handle_output(int handle) override;
    virtual int handle_timeout(int handle) noexcept override;
    //close write
    virtual int handle_close(int handle) override;
    virtual int handle_signal(int handle) override;
    // virtual int get_handle() const {}
    int connect(const net::inet_addr &target_addr, const micro_seconds &timeout);
    int connect_n(const net::inet_addr &target_addr, int n, const micro_seconds &timeout);

    const std::vector<IOHandlerPtr> *get_handlers() const { return handlers_; }

private:
    int open();
    int close();
    Handler *make_handler();
    int activate_io_handler(int handle);
    int connect_i(const net::inet_addr &target_addr, const micro_seconds &timeout);

private:
    thread::thread_pool *pool_;
    MessageQueueType *mq_;
    net::sock_connector connector_;
    std::vector<IOHandlerPtr> handlers_;
};

template <typename DataType, typename Handler>
reactor_connector<DataType, Handler>::reactor_connector(Reactor &react, thread::thread_pool &pool, MessageQueueType &mq)
    : EventHandler(react), pool_(&pool), mq_(&mq), connector_(), handlers_()
{
    this->open();
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::open() { return 0; }

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::close()
{
    return 0;
}

template <typename DataType, typename Handler>
reactor_connector<DataType, Handler>::~reactor_connector() {}

//connect succeed
template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::handle_input(int) { return 0; }

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::handle_timeout(int) noexcept { return 0; }

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::handle_close(int) { return 0; }

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::handle_signal(int) { return 0; }

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::connect(
    const net::inet_addr &target_addr,
    const micro_seconds &timeout)
{
    return connect_i(target_addr, timeout);
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::connect_n(
    const net::inet_addr &target_addr,
    int n, const micro_seconds &timeout)
{
    (void)target_addr;
    (void)n;
    (void)timeout;
    return 0;
}

template <typename DataType, typename Handler>
Handler *reactor_connector<DataType, Handler>::make_handler()
{
    //handler sock_stream
    auto *handler = new Handler(*reactor_, *pool_, *mq_);
    if (handler == 0)
        return nullptr;

    // handlers_[handle].reset(handler);
    // return handler->get_handle();
    return handler;
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::activate_io_handler(int handle)
{
    //set non-blocking
    if (handlers_[handle]->get_sock_stream().get_sock_fd().set_non_blocking() != 0)
    {
        LOG(WARNING) << "handle: " << handle << " setting non-blocking error" << strerror(errno);
        handlers_[handle]->close();
        handlers_[handle].reset();
        return -1;
    }

    //open handler
    if (handlers_[handle]->open() != 0)
    {
        LOG(WARNING) << "handle: " << handle << " open error" << strerror(errno);
        handlers_[handle]->close();
        handlers_[handle].reset();
        return -1;
    }
    return 0;
}

template <typename DataType, typename Handler>
int reactor_connector<DataType, Handler>::connect_i(const net::inet_addr &target_addr, const micro_seconds &timeout)
{
    //make handler
    auto *newHandler = make_handler();

    auto &stream = newHandler->get_sock_stream();
    //connect will set the stream's fd
    if (connector_.connect(stream, target_addr, &timeout, 1, 0) != 0)
    {
        LOG(WARNING) << "connect to " << target_addr.get_address_string() << " error...";
        newHandler->close();
        return -1;
    }

    //record the handle in handlers_
    int handle = stream.get_handle();
    if (handlers_.size() < static_cast<size_t>(handle))
        handlers_.resize(handle * 2);
    handlers_[handle].reset(newHandler);

    //activate handler: bind to reactor
    return activate_io_handler(handle);
}
} // namespace reactor

#endif // _UNP_REACTOR_CONNECTOR_H_
