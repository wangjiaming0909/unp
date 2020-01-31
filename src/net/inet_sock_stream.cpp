#include "inet_sock_stream.h"
#include <sys/uio.h>

namespace net
{
ssize_t InetSockStream::read(void *buffer, size_t len)
{
    return read_imp(buffer, len);
}

ssize_t InetSockStream::read(reactor::buffer& buf, size_t len)
{
    char* const data_p = static_cast<char*>(::calloc(len + 1, 1));
    int read_len = read_imp(data_p, len);
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

ssize_t InetSockStream::readv(iovec iov[], int n)
{
    return readv_imp(iov, n);
}

ssize_t InetSockStream::send(const void *buffer, size_t len, int flags)
{
    return send_imp(buffer, len, flags);
}

ssize_t InetSockStream::write(const void* buffer, size_t len)
{
	if(sock_fd_.get_handle() == INVALID_HANDLE) return 0;
	int write_len = 0;
	write_len = ::write(sock_fd_.get_handle(), buffer, len);
	// LOG(INFO) << "Send to: " << sock_fd_->get_handle() << " send: " << len << "bytes";
	return write_len;
}

ssize_t InetSockStream::writev(const iovec iov[], int n)
{
    return writev_imp(iov, n);
}

/*
ssize_t InetSockStream::recv_n(void* buffer, size_t len, int flags,
		const micro_seconds* timeoute, size_t* bytes_transfered) const{
//TODO ACE.cpp line 577 recv_n_i
}

ssize_t InetSockStream::send_n(const void* buffer, size_t len, int flags, 
		const micro_seconds* timeout, size_t* bytes_transfered) const{

}
*/
ssize_t InetSockStream::recv(void *, size_t, int)
{
    return 0;
}
ssize_t InetSockStream::recv_n(void *, size_t, int)
{
    return 0;
}
ssize_t InetSockStream::readv_n(iovec [], size_t)
{
    return 0;
}
ssize_t InetSockStream::send_n(const void *, size_t, int)
{
    return 0;
}
ssize_t InetSockStream::writev_n(const void *, size_t)
{
    return 0;
}
}
