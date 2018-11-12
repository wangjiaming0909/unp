#include "sock_stream.h"

ssize_t net::sock_stream::recv( void* buffer, size_t len, 
    const micro_seconds* timeout) const{
    return recv_imp(buffer, len, timeout);
}
ssize_t net::sock_stream::recvv(
    iovec iov[], int n,
    const micro_seconds* timeout)const{
}

ssize_t net::sock_stream::send( const void* buffer, size_t len, int flags,
    const micro_seconds* timeout)const{
    return ::send(
            this->sock_fd_->get_handler(), buffer, len, flags);
}

ssize_t net::sock_stream::sendv(
    const iovec iov[], int n,
    const micro_seconds* timeout) const{
}


ssize_t net::sock_stream::recv_imp(void* buffer, size_t len,
    const micro_seconds* timeout) const{
	int ret = 0;
    if(timeout == 0){
        return ret = ::read(sock_fd_->get_handler(), buffer, len);
    } else {
        //TODO use nonblocking read, wait for timeout with poll
        auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
        unp::handle_read_ready_using_poll(sock_fd_->get_handler(), milli_seconds);
        auto old_flag = sock_fd_->fcntl(F_GETFL, 0);
//        old_flag |= O_NONBLOCK;
        SET_BIT(old_flag, O_NONBLOCK);
        sock_fd_->fcntl(F_SETFL, old_flag);
        ret = ::read(sock_fd_->get_handler(), static_cast<char*>(buffer), len);
//        old_flag &= ~O_NONBLOCK;
        CLR_BIT(old_flag, O_NONBLOCK);
        sock_fd_->fcntl(F_SETFL, old_flag);
    }
	return ret;
}
