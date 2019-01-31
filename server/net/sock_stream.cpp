#include "sock_stream.h"

void net::sock_stream::close_reader()
{
    if (sock_fd_ != 0)
        sock_fd_->shut_down(SHUT_RD);
}

void net::sock_stream::close_writer()
{
    if (sock_fd_ != 0)
        sock_fd_->shut_down(SHUT_WR);
}

void net::sock_stream::close()
{
    if (sock_fd_ != 0)
        if (sock_fd_->close() != 0)
        {
            LOG(WARNING) << "close error... " << strerror(errno);
        }
}

ssize_t net::sock_stream::read(void *buffer, size_t len,
                               const micro_seconds *timeout) const
{
    return read_imp(buffer, len, timeout);
}

ssize_t net::sock_stream::readv(iovec iov[], int n,
                                const micro_seconds *timeout) const
{
    return readv_imp(iov, n, timeout);
}

ssize_t net::sock_stream::send(const void *buffer, size_t len, int flags,
                               const micro_seconds *timeout) const
{
    return send_imp(buffer, len, flags, timeout);
}

ssize_t net::sock_stream::writev(const iovec iov[], int n,
                                 const micro_seconds *timeout) const
{
    return writev_imp(iov, n, timeout);
}

/*
ssize_t net::sock_stream::recv_n(void* buffer, size_t len, int flags,
		const micro_seconds* timeoute, size_t* bytes_transfered) const{
//TODO ACE.cpp line 577 recv_n_i
}

ssize_t net::sock_stream::send_n(const void* buffer, size_t len, int flags, 
		const micro_seconds* timeout, size_t* bytes_transfered) const{

}
*/

ssize_t net::sock_stream::read_imp(void *buffer, size_t len,
                                   const micro_seconds *timeout) const
{
    if (sock_fd_ == 0)
        return 0;
    int ret = 0;
    if (timeout == 0)
    {
        return ret = ::read(sock_fd_->get_handle(), buffer, len);
        LOG(INFO) << "read from: " << sock_fd_->get_handle() << " read: " << len << "bytes";
    }
    else
    {
        //TODO use nonblocking read, wait for timeout with poll
        auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
        unp::handle_read_ready_using_poll(sock_fd_->get_handle(), milli_seconds);
        sock_fd_->set_non_blocking();
        ret = ::read(sock_fd_->get_handle(), static_cast<char *>(buffer), len);
        LOG(INFO) << "read from: " << sock_fd_->get_handle() << " read: " << len << "bytes";
        sock_fd_->restore_blocking();
    }
    return ret;
}

ssize_t net::sock_stream::send_imp(const void *buffer, size_t len, int flags, const micro_seconds *timeout) const
{
    if (sock_fd_ == 0)
        return 0;
    int ret = 0;
    if (timeout == 0)
    {
        LOG(INFO) << "send to: " << sock_fd_->get_handle() << " send: " << len << "bytes";
        return ::send(sock_fd_->get_handle(), buffer, len, flags);
    }
    else
    {
        auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
        unp::handle_write_ready_using_poll(sock_fd_->get_handle(), milli_seconds);
        sock_fd_->set_non_blocking();
        ret = ::send(sock_fd_->get_handle(), buffer, len, flags);
        LOG(INFO) << "send to: " << sock_fd_->get_handle() << " send: " << len << "bytes";
        sock_fd_->restore_blocking();
    }
    return ret;
}

ssize_t net::sock_stream::readv_imp(iovec iov[], int n, const micro_seconds *timeout) const
{
    if (sock_fd_ == 0)
        return 0;
    int ret = 0;
    if (timeout == 0)
    {
        return ::readv(sock_fd_->get_handle(), iov, n);
        LOG(INFO) << "readv from: " << sock_fd_->get_handle();
    }
    else
    {
        auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
        unp::handle_read_ready_using_poll(sock_fd_->get_handle(), milli_seconds);
        sock_fd_->set_non_blocking();
        ret = ::readv(sock_fd_->get_handle(), iov, n);
        LOG(INFO) << "readv from: " << sock_fd_->get_handle();
        sock_fd_->restore_blocking();
    }
    return ret;
}

ssize_t net::sock_stream::writev_imp(const iovec iov[], int n, const micro_seconds *timeout) const
{
    if (sock_fd_ == 0)
        return 0;
    int ret = 0;
    if (timeout == 0)
    {
        LOG(INFO) << "writev to: " << sock_fd_->get_handle();
        return ::writev(sock_fd_->get_handle(), iov, n);
    }
    else
    {
        auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
        unp::handle_read_ready_using_poll(sock_fd_->get_handle(), milli_seconds);
        sock_fd_->set_non_blocking();
        ret = ::writev(sock_fd_->get_handle(), iov, n);
        LOG(INFO) << "writev to: " << sock_fd_->get_handle();
        sock_fd_->restore_blocking();
    }
    return ret;
}
