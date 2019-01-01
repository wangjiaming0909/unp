#include "acceptor.h"

reactor::reactor_acceptor:: reactor_acceptor(Reactor& react, const net::inet_addr& local_addr)
        : event_handler(react), 
          acceptor_(local_addr), 
          local_addr_(local_addr){ 
    this->open();
}

reactor::reactor_acceptor::~reactor_acceptor(){ }

int reactor::reactor_acceptor::open(){
    int ret = 0;
    if((ret = acceptor_.open(local_addr_, REUSE_ADDR)) < 0){
        LOG(ERROR) << "sock_acceptor open error..." << strerror(errno);
        return ret;
    }
    //open succeed
    //register to the reactor
    reactor_->register_handler(this, event_handler::ACCEPT_EVENT);
    return 0;
}

int reactor::reactor_acceptor::close(){
    return acceptor_.close();
}

int reactor::reactor_acceptor::handle_input(int handle){
    
}
int reactor::reactor_acceptor::handle_timeout(int handle){

}
int reactor::reactor_acceptor::handle_close(int handle){

}
int reactor::reactor_acceptor::handle_signal(int handle){

}
