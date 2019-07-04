#ifndef UNP_REACTOR_ACCEPTOR_H_
#define UNP_REACTOR_ACCEPTOR_H_

#include "reactor/EventHandler.h"
#include "thread/thread_pool.h"
#include "net/inet_addr.h"
#include "net/sock_acceptor.h"
#include "reactor/read_handler.h"
#include "thread/message_queue.h"
#include "reactor/reactor.h"
#include "reactor/acceptor.h"
#include <vector>
#include <memory>

using namespace std::chrono_literals;

namespace reactor{

template <typename DataType, typename Handler>
class ReactorAcceptor : public EventHandler
{
public:
    using mqType = message_queue<DataType>;
    using RHandlerType = Handler;

    ReactorAcceptor(
        Reactor& react, thread::thread_pool& pool, 
        message_queue<DataType>& mq, const net::inet_addr& local_addr);
    ~ReactorAcceptor() override;
    virtual int handle_input(int handle) override;
    // virtual int handle_timeout(int handle) override;
    virtual int handle_close(int handle) override;
    // virtual int handle_signal(int handle) override;
    virtual int get_handle() const override {return acceptor_.get_handle();}

private:
    int activate_read_handler();
    int open();
    int close(){ return acceptor_.close(); }

private:
    using RHandlerPtr = std::shared_ptr<RHandlerType>;

private:
    mqType                          *mq_;
    thread::thread_pool             *pool_;
    net::sock_acceptor	            acceptor_;
    net::inet_addr                  local_addr_;
    std::vector<RHandlerPtr>        read_handlers_;
};

template <typename DataType, typename Handler>
ReactorAcceptor<DataType, Handler>:: ReactorAcceptor( Reactor& react
    , thread::thread_pool& pool
    , message_queue<DataType>& mq
    , const net::inet_addr& local_addr)
    : EventHandler(react)
    , mq_(&mq)
    , pool_(&pool)
    , acceptor_(local_addr)
    , local_addr_(local_addr)
    , read_handlers_()
{
    this->open();
}

template <typename DataType, typename Handler>
ReactorAcceptor<DataType, Handler>::~ReactorAcceptor() { }

template <typename DataType, typename Handler>
int ReactorAcceptor<DataType, Handler>::open()
{
    int ret = 0;
    if((ret = acceptor_.open(local_addr_, REUSE_ADDR)) < 0){
        LOG(ERROR) << "sock_acceptor open error..." << strerror(errno);
        return ret;
    }
    //open succeed, register to the reactor
    return reactor_->register_handler(acceptor_.get_handle(), this, EventHandler::ACCEPT_EVENT);
}

template <typename DataType, typename Handler>
int ReactorAcceptor<DataType, Handler>::handle_input(int handle){
    LOG(INFO) << "start to accepting a connection";
    pool_->start();
    return activate_read_handler();
}

template <typename DataType, typename Handler>
int ReactorAcceptor<DataType, Handler>::activate_read_handler(){
    net::inet_addr peer{};
    ReadHandler<int> tmpHandler{reactor_, mq_, pool_};

    if( this->acceptor_.accept(tmpHandler.get_sock_stream(), &peer) != 0) 
    {
        LOG(ERROR) << "accept error..." << strerror(errno);
        return -1;
    }

    LOG(INFO) << "accepted...";

    int handle = tmpHandler.get_handle();
    if(read_handlers_.size() < handle + 1){
        read_handlers_.resize(handle + 1);
    }

    read_handlers_[handle].reset(new ReadHandler<int>(std::move(tmpHandler)));

    //if one handle set non-blocking or open error, return 0
    //the acceptor should continue listening, ignore the handle which is error
    if(read_handlers_[handle]->get_sock_stream().get_sock_fd().set_non_blocking() != 0)
    {
        LOG(WARNING) << "setting nonblock error....";
        read_handlers_[handle].reset(nullptr);
        return 0;
    }

    if(read_handlers_[handle]->open() != 0)
    {
        LOG(WARNING) << "handle: " << handle << " open error..." << strerror(errno);
        read_handlers_[handle]->close();
        read_handlers_[handle].reset(nullptr);
        return 0;
    }
    return 0;
}

template <typename DataType, typename Handler>
int ReactorAcceptor<DataType, Handler>::handle_close(int handle){ 
    (void)handle;
    this->acceptor_.close();
    this->pool_->stop(5s);
	return 0;
}

// template <typename DataType>
// int ReactorAcceptor<DataType>::handle_timeout(int handle){ }

}//reactor
#endif // UNP_REACTOR_ACCEPTOR_H_