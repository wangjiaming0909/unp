#include "ssl_sock_stream.h"
#include <openssl/err.h>
#include "reactor/buffer.h"
namespace net
{

  OpenSSLInitializer::OpenSSLInitializer()
  {
    SSL_library_init();
    //method = DTLS_client_method();
    method = SSLv23_client_method();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_algorithms();
  }

OpenSSLInitializer SSLSockStream::sslInitializer_ = OpenSSLInitializer{};
SSLSockStream::~SSLSockStream()
{
	freeSSL();
}

int SSLSockStream::connect()
{
    SSL_set_connect_state(ssl_);
    return doHandShake();
}

int SSLSockStream::accept()
{
    SSL_set_accept_state(ssl_);
    return doHandShake();
}

ssize_t SSLSockStream::read(void *buffer, size_t len)
{
    return read_imp(buffer, len);
}

ssize_t SSLSockStream::read(reactor::buffer& buf, size_t len)
{
    char* const data_p = static_cast<char*>(::calloc(len + 1, 1));
    if(!can_read()) return -1;
    int read_len = 0;
    while( (read_len = SSL_read(ssl_, data_p, len)) > 0)
    {
        auto data_appended = buf.append(data_p, read_len);
        memset(data_p, 0, len + 1);
        if(data_appended != read_len)
        {
            // LOG(WARNING) << "Read data from socket len: " << read_len << " appended into buffer len: " << data_appended;
            free(data_p);
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

ssize_t SSLSockStream::send(const void *buffer, size_t len, int)
{
    // return send_imp(buffer, len, flags);
    if(!can_write()) return 0;
    return SSL_write(ssl_, buffer, len);
}

ssize_t SSLSockStream::write(const void* buffer, size_t len)
{
	if(fd_->get_fd() == INVALID_HANDLE || !can_write()) return 0;
	int write_len = 0;
	write_len = ::write(fd_->get_fd(), buffer, len);
	// LOG(INFO) << "Send to: " << sock_fd_->get_handle() << " send: " << len << "bytes";
	return write_len;
}

int SSLSockStream::open()
{
	//open sockfd will create a socket fd
	return (SockStream::open() == 0) && initSSL();
}

int SSLSockStream::initSSL()
{
    try {
        method_ = sslInitializer_.method;
        sslctx_ = SSL_CTX_new(method_);
        ssl_ = SSL_new(sslctx_);

        SSL_set_fd(ssl_, fd_->get_fd());
    } catch(...) {
        freeSSL();
        return -1;
    }
    return 0;
}

int SSLSockStream::set_handle(int handle)
{
    return (SockStream::set_handle(handle) >= 0) && initSSL();
}

int SSLSockStream::doHandShake()
{
    int t = 3;
    while (t)
    {
        ERR_clear_error();
        auto r = SSL_do_handshake(ssl_);
        LOG(DEBUG) << "ssl do handshake ret: " << r;
        if(r == 1) break;

        int err = SSL_get_error(ssl_, r);
        switch (err)
        {
        case SSL_ERROR_WANT_READ:
            //can't read unitl has more data
            LOG(ERROR) << "handshake error WANT READ...";
            break;
        case SSL_ERROR_WANT_WRITE:
            LOG(ERROR) << "handshake error WANT WRITE...";
            break;
        case SSL_ERROR_SSL:
            LOG(ERROR) << "SSL fatal error...";
            SSL_shutdown(ssl_);
            return -1;
        default:
            {
              while(err != 0) {
                LOG(ERROR) << "handshake error: " << err << " errno: " << strerror(errno);
                err = ERR_get_error();
              }
              break;
            }
        }
        t--;
        LOG(WARNING) << "handshake retrying time: " << 4 - t;
        continue;
    }
    return t != 0 ? 0 : -1;
}
void SSLSockStream::freeSSL()
{
	//TODO what about method_
    if(ssl_) SSL_free(ssl_);
    if(sslctx_) SSL_CTX_free(sslctx_);
}

ssize_t SSLSockStream::writev(const iovec iov[], int n)
{
    return writev_imp(iov, n);
}

ssize_t SSLSockStream::recv(void *, size_t, int)
{
    return 0;
}
ssize_t SSLSockStream::recv_n(void *, size_t, int)
{
    return 0;
}
ssize_t SSLSockStream::readv_n(iovec [], size_t)
{
    return 0;
}
ssize_t SSLSockStream::send_n(const void *, size_t, int)
{
    return 0;
}
ssize_t SSLSockStream::writev_n(const void *, size_t)
{
    return 0;
}

}
