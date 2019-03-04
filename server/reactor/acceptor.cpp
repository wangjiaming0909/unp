#include "server/reactor/acceptor.h"

using namespace reactor;

reactor_acceptor:: reactor_acceptor(
    Reactor& react, thread_pool& pool,
    const net::inet_addr& local_addr)
        : event_handler(react)
        , acceptor_(local_addr)
        , local_addr_(local_addr)
        , mq_()
        , pool_(pool)
        , read_handler_(react, pool_, mq_)
        , read_handlers_() { this->open(); }

reactor_acceptor::~reactor_acceptor(){ }

int reactor_acceptor::open(){
    int ret = 0;
    if((ret = acceptor_.open(local_addr_, REUSE_ADDR)) < 0){
        LOG(ERROR) << "sock_acceptor open error..." << strerror(errno);
        return ret;
    }
    //open succeed, register to the reactor
    return reactor_->register_handler(acceptor_.get_handle(), this, event_handler::ACCEPT_EVENT);
}

int reactor_acceptor::handle_input(int handle){
    LOG(INFO) << "start to accept a connection";
    // pool_.start();
    do{
        activate_read_handler();
    }while(unp::handle_read_ready_using_poll(handle, 2ms) == 1);
    // read_handler_.activate(1);
    return 0;
}

void reactor_acceptor::activate_read_handler(){
    net::inet_addr peer{};
    ReadHandler<int> tmpHandler{*read_handler_.get_reactor(), *read_handler_.pool(), *read_handler_.mq()};
    int ret = this->acceptor_.accept(tmpHandler.get_sock_stream(), &peer);
    if(ret == 0) 
        LOG(INFO) << "accepted...";
    if(ret != 0){
        LOG(ERROR) << "accept error..." << strerror(errno);
    }

    int handle = tmpHandler.get_handle();
    if(read_handlers_.size() < handle + 1){
        read_handlers_.resize(handle + 1);
    }

    read_handlers_[handle].reset(new ReadHandler<int>(std::move(tmpHandler)));
    if(ret != 0 && read_handlers_[handle]->get_sock_stream().get_sock_fd().set_non_blocking() != 0)
        LOG(WARNING) << "setting nonblock error....";
    read_handlers_[handle]->open();
    // if(ret != 0 && read_handler_.get_sock_stream().get_sock_fd().set_non_blocking() != 0){
    //     LOG(WARNING) << "setting nonblock error";
    // }
    // read_handler_.open();
}


int reactor_acceptor::handle_timeout(int handle){ }
int reactor_acceptor::handle_close(int handle){ }
int reactor_acceptor::handle_signal(int handle){ }