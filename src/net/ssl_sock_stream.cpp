#include "ssl_sock_stream.h"

namespace net 
{
ssize_t SSLSockStream::read(void *buffer, size_t len)
{
    return read_imp(buffer, len);
}

ssize_t SSLSockStream::read(reactor::buffer& buf, size_t len)
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

ssize_t SSLSockStream::readv(iovec iov[], int n)
{
    return readv_imp(iov, n);
}

ssize_t SSLSockStream::send(const void *buffer, size_t len, int flags)
{
    return send_imp(buffer, len, flags);
}

ssize_t SSLSockStream::write(const void* buffer, size_t len)
{
	if(sock_fd_.get_handle() == INVALID_HANDLE) return 0;
	int write_len = 0;
	write_len = ::write(sock_fd_.get_handle(), buffer, len);
	// LOG(INFO) << "Send to: " << sock_fd_->get_handle() << " send: " << len << "bytes";
	return write_len;
}

ssize_t SSLSockStream::writev(const iovec iov[], int n)
{
    return writev_imp(iov, n);
}

ssize_t SSLSockStream::recv(void *buffer, size_t len, int flags)
{

}
ssize_t SSLSockStream::recv_n(void *buffer, size_t len, int flags)
{

}
ssize_t SSLSockStream::readv_n(iovec iov[], size_t n)
{

}
ssize_t SSLSockStream::send_n(const void *buffer, size_t len, int flags)
{

}
ssize_t SSLSockStream::writev_n(const void *buffer, size_t len)
{

}

}