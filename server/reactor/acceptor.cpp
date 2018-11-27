#include "acceptor.h"
reactor::reactor_acceptor:: reactor_acceptor(
        const net::inet_addr& local_addr, 
        IO_TYPE io_type)
    : io_type_(io_type)
    , acceptor_(local_addr)
    , local_addr_(local_addr){ }

reactor::reactor_acceptor::~reactor_acceptor(){ }

int reactor::reactor_acceptor::get_handle() const{
    return acceptor_.get_handle();
}
int reactor::reactor_acceptor::open(){
    return acceptor_.open(local_addr_, 1);
}

int reactor::reactor_acceptor::close(){
    return acceptor_.close();
}