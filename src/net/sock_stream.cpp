#include "sock_stream.h"
#include <sys/uio.h>

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
	if(sock_fd_ == nullptr) return;

	if (sock_fd_->close() != 0)
	{
		LOG(WARNING) << "close error... " << strerror(errno);
	}
}

ssize_t net::sock_stream::read(void *buffer, size_t len, const micro_seconds *timeout) const
{
    return read_imp(buffer, len, timeout);
}

ssize_t net::sock_stream::read(reactor::buffer& buf, size_t len, const microseconds* timeout) const
{
    char* const data_p = static_cast<char*>(::calloc(len + 1, 1));
    int read_len = read_imp(data_p, len, timeout);
    if(read_len > 0 )
    {
        auto data_appended = buf.append(data_p, read_len);
        if(data_appended != read_len)
        {
            // LOG(WARNING) << "Read data from socket len: " << read_len << " appended into buffer len: " << data_appended;
            return -1;
        }
    }
    free(data_p);
    return read_len;
}

ssize_t net::sock_stream::readv(iovec iov[], int n, const micro_seconds *timeout) const
{
    return readv_imp(iov, n, timeout);
}

ssize_t net::sock_stream::send(const void *buffer, size_t len, int flags, const micro_seconds *timeout) const
{
    return send_imp(buffer, len, flags, timeout);
}

ssize_t net::sock_stream::write(const void* buffer, size_t len, const micro_seconds* timeout) const
{
	if(sock_fd_ == 0) return 0;
	int write_len = 0;
	if(timeout == nullptr)
	{
		write_len = ::write(sock_fd_->get_handle(), buffer, len);
		// LOG(INFO) << "Write to handle: " << sock_fd_->get_handle() << " " << write_len << " bytes";
		return write_len;
	}

	auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
	unp::handle_write_ready_using_poll(sock_fd_->get_handle(), milli_seconds);
	sock_fd_->set_non_blocking();
	write_len = ::write(sock_fd_->get_handle(), buffer, len);
	// LOG(INFO) << "Send to: " << sock_fd_->get_handle() << " send: " << len << "bytes";
	sock_fd_->restore_blocking();

	return write_len;
}

ssize_t net::sock_stream::writev(const iovec iov[], int n, const micro_seconds *timeout) const
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

//TODO SIGPIPE and Segament fault

ssize_t net::sock_stream::read_imp(void *buffer, size_t len, const micro_seconds *timeout) const
{
    if (sock_fd_ == 0)
        return 0;
    int read_len = 0;
    if (timeout == 0)
    {
        read_len = ::read(sock_fd_->get_handle(), buffer, len);
        // LOG(INFO) << "Read from: " << sock_fd_->get_handle() << " read: " << read_len << "bytes";
        return read_len;
    }
    else
    {
        //TODO use nonblocking read, wait for timeout with poll
        auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
        //?? if the handle is not read ready, we will wait for timeout
        //?? if a connection is very busy(lots of data to read), 
        //?? when we are waiting for the read event, the blocked thread will be waked up
        //?? so the read return EAGAIN or EWOULDBLOCK will only happen at the last read
        //** when we have a lot of small data to read in a very short time,
        //** cause we have more than one thread waiting for the read event
        //** it will be much more fast

        //?? if the timeout is very short, we will waste a lot of cpu time to handle read return -1
        read_len = unp::handle_read_ready_using_poll(sock_fd_->get_handle(), milli_seconds);
        if(read_len <= 0) return -1;
        sock_fd_->set_non_blocking();
        read_len = ::read(sock_fd_->get_handle(), static_cast<char *>(buffer), len);
        if(read_len > 0) 
        {
            // LOG(INFO) << "Read from: " << sock_fd_->get_handle() << " read: " << read_len << "bytes";
        } 
        else if(read_len == 0)
        {
            // LOG(INFO) << "Read a EOF: " << sock_fd_->get_handle();
        }
        else
        {
            LOG(INFO) << "Read from: " << sock_fd_->get_handle() << " error: " << strerror(errno);
        }
        sock_fd_->restore_blocking();
    }
    return read_len;
}

ssize_t net::sock_stream::send_imp(const void *buffer, size_t len, int flags, const micro_seconds *timeout) const
{
    if (sock_fd_ == 0)
        return 0;
    int read_len = 0;
    if (timeout == 0)
    {
        LOG(INFO) << "Send to: " << sock_fd_->get_handle() << " send: " << len << "bytes";
        return ::send(sock_fd_->get_handle(), buffer, len, flags);
    }
    else
    {
        auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
        unp::handle_write_ready_using_poll(sock_fd_->get_handle(), milli_seconds);
        sock_fd_->set_non_blocking();
        read_len = ::send(sock_fd_->get_handle(), buffer, len, flags);
        LOG(INFO) << "Send to: " << sock_fd_->get_handle() << " send: " << len << "bytes";
        sock_fd_->restore_blocking();
    }
    return read_len;
}

ssize_t net::sock_stream::readv_imp(iovec iov[], int n, const micro_seconds *timeout) const
{
    if (sock_fd_ == 0)
        return 0;
    int read_len = 0;
    if (timeout == 0)
    {
        return ::readv(sock_fd_->get_handle(), iov, n);
        // LOG(INFO) << "Readv from: " << sock_fd_->get_handle();
    }
    else
    {
        auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
        unp::handle_read_ready_using_poll(sock_fd_->get_handle(), milli_seconds);
        sock_fd_->set_non_blocking();
        read_len = ::readv(sock_fd_->get_handle(), iov, n);
        // LOG(INFO) << "Readv from: " << sock_fd_->get_handle();
        sock_fd_->restore_blocking();
    }
    return read_len;
}

ssize_t net::sock_stream::writev_imp(const iovec iov[], int n, const micro_seconds *timeout) const
{
    if (sock_fd_ == 0)
        return 0;
    int read_len = 0;
    if (timeout == 0)
    {
        // LOG(INFO) << "Writev to: " << sock_fd_->get_handle();
        return ::writev(sock_fd_->get_handle(), iov, n);
    }
    else
    {
        auto milli_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
        unp::handle_read_ready_using_poll(sock_fd_->get_handle(), milli_seconds);
        sock_fd_->set_non_blocking();
        read_len = ::writev(sock_fd_->get_handle(), iov, n);
        // LOG(INFO) << "Writev to: " << sock_fd_->get_handle();
        sock_fd_->restore_blocking();
    }
    return read_len;
}