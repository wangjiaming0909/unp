#include "sock_acceptor.h"


// net::sock_acceptor::sock_acceptor(){}

net::sock_acceptor::~sock_acceptor(){}

net::sock_acceptor::sock_acceptor(
            const inet_addr& local_addr,
            int reuse_addr,
            int protocol_family,
            int backlog,
            int protocol){
    sock_fd_ = boost::make_shared<inet_sock>();
    (void)reuse_addr;
    (void)protocol_family;
    (void)backlog;
    (void)protocol;
    (void)local_addr;
    // if(this->open(local_addr, reuse_addr, protocol_family, backlog, protocol) == -1){
    //     LOG(ERROR) << "sock_accept: func open error" ;
    // }
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
        LOG(ERROR) << "sock_fd open error ..." << strerror(errno);
        return -1;
    }else{
    //then bind, listen
        return this->shared_open(local_addr, 
                                protocol_family, 
                                backlog);
    }
}

int net::sock_acceptor::close(){
    return sock_fd_->close();
}

int net::sock_acceptor::accept(
            sock_stream& client_stream,
            inet_addr* remote_addr,
            microseconds* timeout,
            bool restart) const
{
    //non-blocking accept
    int in_blocking_mode = 0;
    int ret = this->shared_accept_start(timeout, restart, in_blocking_mode);
    if(ret  == -1) return -1;

    socklen_t len = 0;
    socklen_t *len_ptr = 0;
    sockaddr addr;
    // sockaddr* addr = 0;
    if(remote_addr ){
        len = remote_addr->get_size();
        len_ptr = &len;
        memcpy(&addr, (sockaddr*)remote_addr->get_sockaddr_in_ptr().get(), sizeof(sockaddr));
        // addr = (sockaddr*)remote_addr->get_sockaddr_in_ptr().get();//!ERROR
    }

    for(;;){
        LOG(INFO) << "invoking accept...";
        int client_fd = ::accept(sock_fd_->get_handle(), &addr, len_ptr);
        //restart set and accept failed and it was interrupted, then continue
        if(restart && client_fd == -1 && errno == EINTR){
            LOG(INFO) << "interrupted... restarting.....";
            continue;
        } else if(client_fd == -1) return -1;//other errors
        ret = client_stream.set_handle(client_fd);
        if(remote_addr) {
            auto addr_string = remote_addr->get_address_string();
            LOG(INFO) << "accepted a connection..." << addr_string;
        }
        //accept returned successfully, remote addr was set, addrlen was set too
        //and we write it to the remote_addr pointer
        if(ret != INVALID_HANDLE && remote_addr){
            remote_addr->set_size(len);
            // LOG(INFO) << "settings size and family " << len << " " << addr.sa_family;
            break;
        }
    }

    return shared_accept_finish(client_stream, in_blocking_mode);
}

int net::sock_acceptor::shared_open(
        const inet_addr& local_sap,
        int protocol_family,
        int backlog){
    if(protocol_family != AF_INET){
        errno = EINVAL;
        LOG(ERROR) << "protocol_family is not AF_INET";
        return -1;
    }
    auto addr_str = local_sap.get_address_string();
    auto port = local_sap.get_port_number();
    LOG(INFO) << "trying to bind to: " << addr_str << ":" << port << "..." ;
    int ret = ::bind(sock_fd_->get_handle(), 
        local_sap.get_sockaddr_ptr().get(), 
        local_sap.get_size());
    if(ret != 0){
        LOG(ERROR) << "bind error: " << strerror(errno);
        return ret;
    }
    LOG(INFO) << "listening on: " << addr_str << ":" << port << "...";
    ret = ::listen(sock_fd_->get_handle(), backlog);
    if(ret != 0){
        LOG(ERROR) << "listen error: " << strerror(errno);
        return ret;
    }
    return ret;
}

int net::sock_acceptor::shared_accept_start(microseconds *timeout,
                bool restart,
                int& in_blocking_mode) const{
    int ret = 0;
    if(timeout){//using timeout
        auto timeout_milliseconds = std::chrono::duration_cast<milliseconds>(*timeout);
        ret = unp::handle_timed_accept_using_poll(sock_fd_->get_handle(), &timeout_milliseconds, restart);
        if(ret == -1) return -1;
        else{// ret != -1 then re == 0
            //means succeed, check the blocking mode, 
            //set to non-blocking mode for the next accept method
            in_blocking_mode = BIT_ENABLED(sock_fd_->get_flags(), O_NONBLOCK);
            if(!(in_blocking_mode && (sock_fd_->set_flags(F_SETFL, O_NONBLOCK)) == -1))
                return -1;
        }
    }
    return 0;
}

//set two handle to non-blocking mode
int net::sock_acceptor::shared_accept_finish(
        sock_stream& client_stream,
        bool in_blocking_mode) const
{
    int ret = 0;
    if(in_blocking_mode){
        ret = sock_fd_->restore_blocking();
        if(ret == -1) return ret;
        ret = client_stream.get_sock_fd().restore_blocking();
    }
    return ret;
}

int net::sock_acceptor::get_handle() const{
    return sock_fd_->get_handle();
}
