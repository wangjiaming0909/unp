#ifndef _UNP_SOCK_STREAM_H_
#define _UNP_SOCK_STREAM_H_
#include <chrono>
#include <sys/socket.h>
#include "inet_sock.h"
#include <unistd.h>
#include "unp.h"
#include "macros.h"
#include <boost/assert.hpp>
#include "server/util/easylogging++.h"
#include "server/reactor/buffer.h"
namespace net{

//sock_stream does not contain the memory of the sock_fd
//when need to use non-blocking read and write 
//need to change the status of the fd first
//then use sock_stream
class sock_stream{
public:
	sock_stream() : sock_fd_(new inet_sock()){}
	sock_stream(sock_stream&& other) {
		this->sock_fd_ = other.sock_fd_;
		other.sock_fd_ = nullptr;
	}
	~sock_stream() { delete sock_fd_; }
    sock_stream(const sock_stream& other) = delete ;
public:
	void close_reader();
	void close_writer();
	void close();
public:
    using micro_seconds = std::chrono::microseconds; 
	//read version use system call read which has no flags 
    ssize_t read( void* buffer, size_t len, const micro_seconds* timeout = 0) const;
	ssize_t read(reactor::buffer& buf, size_t len, const microseconds* timeout = 0) const;
	//TODO
	ssize_t write(const void* buffer, size_t len, const micro_seconds* timeout = 0) const;
	//recv version use the system call recv which has a flags parameter

	//TODO
	ssize_t recv(void* buffer, size_t len, int flags, const micro_seconds* timeout = 0) const;
    ssize_t readv( iovec iov[], int n, const micro_seconds* timeout = 0)const;
	//send alse has a flags parameter
    ssize_t send( const void* buffer, size_t len, int flags, const micro_seconds* timeout = 0)const;
    ssize_t writev( const iovec iov[], int n, const micro_seconds* timeout = 0) const;

public:
	int get_handle() const { return sock_fd_->get_handle(); }
	int set_handle(int handle){
        sock_fd_->set_handle(handle);
        return sock_fd_->get_handle();
	}
	bool has_handle(){
        return sock_fd_->get_handle() != INVALID_HANDLE;
	}
	int open_sock_fd(int family, sock_type type, int protocol, int reuse_addr){
		return sock_fd_->open(family, type, protocol, reuse_addr);
	}
	inet_sock& get_sock_fd() const{ return *sock_fd_; }

public:
	//TODO need implementation
	ssize_t recv_n(void *buffer, size_t len, int flags, 
		const micro_seconds* timeout = 0, size_t *bytes_transfered = 0) const;
	ssize_t readv_n(iovec iov[], size_t n,
		const micro_seconds* timeout = 0, size_t *bytes_transfered = 0) const;
	ssize_t send_n(const void *buffer, size_t len, int flags,
		const micro_seconds* timeout = 0, size_t *bytes_transfered = 0) const;
	ssize_t writev_n(const void *buffer, size_t len, 
		const micro_seconds *timeout, size_t *bytes_transfered = 0) const;

private:
    ssize_t read_imp(void* buffer, size_t len,
        const micro_seconds* timeout = 0) const;
	ssize_t send_imp(const void* buffer, size_t len, int flags,
		const micro_seconds* timeout = 0) const;
	ssize_t readv_imp(iovec iov[], int n, 
		const micro_seconds* timeout = 0)const;
	ssize_t writev_imp(const iovec iov[], int n,
		const micro_seconds* timeout = 0) const;

private:
    inet_sock* sock_fd_;
};

}

#endif // _UNP_SOCK_STREAM_H_
