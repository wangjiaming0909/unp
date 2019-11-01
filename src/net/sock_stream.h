#pragma once
#include "inet_sock.h"
#include "reactor/buffer.h"
#include "boost/noncopyable.hpp"
#include <chrono>

namespace net
{

class SockStream : public boost::noncopyable{
public:
	SockStream() : sock_fd_{}{}

public:
	virtual ssize_t read(void *buffer, size_t len) = 0;
	virtual ssize_t read(reactor::buffer &buf, size_t len) = 0;
	virtual ssize_t write(const void *buffer, size_t len) = 0;
	 //recv version use the system call recv which has a flags parameter
	virtual ssize_t recv(void *buffer, size_t len, int flags) = 0;
	virtual ssize_t readv(iovec iov[], int n) = 0;
	//send alse has a flags parameter
	virtual ssize_t send(const void *buffer, size_t len, int flags) = 0;
	virtual ssize_t writev(const iovec iov[], int n) = 0;
	virtual ssize_t recv_n(void *buffer, size_t len, int flags) = 0;
	virtual ssize_t readv_n(iovec iov[], size_t n) = 0;
	virtual ssize_t send_n(const void *buffer, size_t len, int flags) = 0;
	virtual ssize_t writev_n(const void *buffer, size_t len) = 0;
	virtual int openSockFD(int family, sock_type type, int protocol, int reuse_addr)
	{
		return sock_fd_.open(family, type, protocol, reuse_addr);
	}

public:
	void close() { sock_fd_.close(); }
	void closeReader() { sock_fd_.shut_down(SHUT_RD); }
	void closeWriter() { sock_fd_.shut_down(SHUT_WR); }
	int getHandle() const { return sock_fd_.get_handle(); }
	//[handle] must be a opened socket handle
	inline int setHandle(int handle);
	bool hasHandle() const { return sock_fd_.get_handle() != INVALID_HANDLE; }
	inet_sock& getSockFD() { return sock_fd_; }
	int setNonBolcking() { return sock_fd_.set_non_blocking(); }
	int restoreBlocking() { return sock_fd_.restore_blocking(); }

protected:
	ssize_t read_imp(void *buffer, size_t len);
	ssize_t send_imp(const void* buffer, size_t len, int flags);
	ssize_t readv_imp(iovec iov[], int n);
	ssize_t writev_imp(const iovec iov[], int n);

protected:
	inet_sock sock_fd_;
};

int SockStream::setHandle(int handle)
{
	if(handle == INVALID_HANDLE) return handle;

	if (sock_fd_.get_handle() != INVALID_HANDLE) sock_fd_.close();
	sock_fd_.set_handle(handle);
	return handle;
}
}