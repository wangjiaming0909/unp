#pragma once
#include <openssl/ssl.h>
#include "sock_stream.h"

namespace net
{

struct OpenSSLInitializer
{
  OpenSSLInitializer()
  {
    SSL_library_init();
    method = DTLS_client_method();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_algorithms();
  }
  const SSL_METHOD *method = nullptr;
};

enum class SSLSockStreamState
{
  SSL_IDLE = 0,
  SSL_OPENED = 1,
  SSL_CONNECTING = 2,
  SSL_ACCEPTING = 3
};

class SSLSockStream : public SockStream
{
public:
  SSLSockStream() : SockStream() {}
  SSLSockStream(int family, int protocol) : SockStream(family, protocol) { }
  ~SSLSockStream();
  //read version use system call read which has no flags
  ssize_t read(void *buffer, size_t len) override;
  ssize_t read(reactor::buffer &buf, size_t len) override;
  ssize_t write(const void *buffer, size_t len) override;
  //recv version use the system call recv which has a flags parameter

  ssize_t recv(void *buffer, size_t len, int flags) override;
  ssize_t readv(iovec iov[], int n) override;
  //send alse has a flags parameter
  ssize_t send(const void *buffer, size_t len, int flags) override;
  ssize_t writev(const iovec iov[], int n) override;

  ssize_t recv_n(void *buffer, size_t len, int flags) override;
  ssize_t readv_n(iovec iov[], size_t n) override;
  ssize_t send_n(const void *buffer, size_t len, int flags) override;
  ssize_t writev_n(const void *buffer, size_t len) override;

  int connect() override;
  int accept() override;
  virtual int open() override;
  int set_handle(int handle) override;

protected:
  int initSSL();
  int doHandShake();
  void freeSSL();

private:
  static OpenSSLInitializer sslInitializer_;
  SSL_CTX* sslctx_ = nullptr;
  SSL* ssl_ = nullptr;
  const SSL_METHOD* method_ = nullptr;
  SSLSockStreamState SSLstate_ = SSLSockStreamState::SSL_IDLE;
};

} // namespace net
