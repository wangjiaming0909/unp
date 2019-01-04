#include "acceptor.h"

using namespace reactor;

reactor_acceptor:: reactor_acceptor(Reactor& react, const net::inet_addr& local_addr)
        : event_handler(react)
        , acceptor_(local_addr)
        , local_addr_(local_addr)
        , mq_()
        , pool_(1)
        , read_handler_(react, mq_, pool_) { this->open(); }

reactor_acceptor::~reactor_acceptor(){ }

int reactor_acceptor::open(){
    int ret = 0;
    if((ret = acceptor_.open(local_addr_, REUSE_ADDR)) < 0){
        LOG(ERROR) << "sock_acceptor open error..." << strerror(errno);
        return ret;
    }
    //open succeed
    //register to the reactor
    reactor_->register_handler(acceptor_.get_handle(), this, event_handler::ACCEPT_EVENT);
    return 0;
}

int reactor_acceptor::close(){ return acceptor_.close();
}

int reactor_acceptor::handle_input(int handle){
    LOG(INFO) << "start to accepting a connection";
    pool_.start();
    // char*p = new char;
    // delete p;
    activate_read_handler();
    read_handler_.activate(1);
    return 0;
}
int reactor_acceptor::handle_timeout(int handle){

}
int reactor_acceptor::handle_close(int handle){

}
int reactor_acceptor::handle_signal(int handle){

}

// template <typename data_type>
// boost::shared_ptr<ReadHandler<data_type>> reactor_acceptor::make_read_handler(){
//     boost::shared_ptr<ReadHandler<data_type> ret{new ReadHandler<data_type>()};
// }
void reactor_acceptor::activate_read_handler(){
    net::inet_addr peer{};
    int ret = this->acceptor_.accept(read_handler_.get_sock_stream(), &peer);
    char* p = new char;
    delete p;
    if(ret == 0) 
        // LOG(INFO) << "accepted...";
    if(ret != 0){
        LOG(ERROR) << "accept error..." << strerror(errno);
    }
    if(ret != 0 && read_handler_.get_sock_stream().get_sock_fd().set_non_blocking() != 0){
        LOG(WARNING) << "seting nonblock error";
    }
    read_handler_.open();
}
