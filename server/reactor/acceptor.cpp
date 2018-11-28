#include "acceptor.h"

reactor::reactor_acceptor:: reactor_acceptor(
        const net::inet_addr& local_addr, 
        IO_TYPE io_type)
    : io_type_(io_type)
    , acceptor_(local_addr)
    , local_addr_(local_addr){ 
    this->open();
}

reactor::reactor_acceptor::~reactor_acceptor(){ }

int reactor::reactor_acceptor::get_handle() const{
    return acceptor_.get_handle();
}
int reactor::reactor_acceptor::open(){
    return acceptor_.open(local_addr_, REUSE_ADDR);
}

int reactor::reactor_acceptor::close(){
    return acceptor_.close();
}

int reactor::reactor_acceptor::handle_input(int handle){
    
}
int reactor::reactor_acceptor::handle_output(int handle){

}
int reactor::reactor_acceptor::handle_timeout(int handle){

}
int reactor::reactor_acceptor::handle_close(int handle){

}
int reactor::reactor_acceptor::handle_signal(int handle){

}