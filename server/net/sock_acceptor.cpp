#include "sock_acceptor.h"


net::sock_acceptor::sock_acceptor(){}

net::sock_acceptor::~sock_acceptor(){}

net::sock_acceptor::sock_acceptor(const inet_addr& local_addr,
            int reuse_addr,
            int protocol_family,
            int backlog,
            int protocol){
    
}

int net::sock_acceptor::open(const inet_addr& local_addr,
                int reuse_addr,
                int protocol_family,
                int backlog,
                int protocol){
    // if(local_addr != inet_addr::addr_any)
    //     protocol_family = local_addr.get_type();
    // else if(protocol_family == PF_UNSPEC)
    //     protocol_family = AF_INET;
    //first open the socket
    if(sock_fd_->open(protocol_family, sock_type::stream, protocol, reuse_addr) == -1){
        return -1;
    }else{
    //then bind, listen
        return this->shared_open(local_addr, 
                                protocol_family, 
                                backlog);
    }
}

int net::sock_acceptor::close(){

}

int net::sock_acceptor::accept(
            sock_stream& new_stream,
            inet_addr* remote_addr,
            microseconds* timeout,
            bool restart,
            bool restart_new_handle) const{
    //non-blocking accept
    
}
int net::sock_acceptor::shared_open(
        const inet_addr& local_sap,
        int protocol_family,
        int backlog){
    int ret = ::bind(sock_fd_->get_handler(), 
        local_sap.get_sockaddr_ptr().get(), 
        local_sap.get_size());
    if(ret != 0){
        LOG(ERROR) << "bind error: " << strerror(errno);
        return ret;
    }
    ret = ::listen(sock_fd_->get_handler(), backlog);
    if(ret != 0){
        LOG(ERROR) << "listen error: " <<strerror(errno);
        return ret;
    }
    return ret;
}