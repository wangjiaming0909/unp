#ifndef _UNP_SOCK_CONNECTOR_H_
#define _UNP_SOCK_CONNECTOR_H_

#include "sock_stream.h"
#include "inet_addr.h"
#include "../util/easylogging++.h"

namespace net{

class sock_connector{
public:
    using micro_seconds = std::chrono::microseconds; 
    sock_connector(){}
    ~sock_connector(){}
	int connect(sock_stream& new_stream,
				const inet_addr& remote_addr,
				const micro_seconds& timeout,
				int reuse_addr, int protocol){
		//socket get the socket fd
		if(this->shared_open(new_stream, 
			remote_addr.get_type(), 
			protocol, reuse_addr) == -1){
			return -1;
		}
		new_stream.get_sock_fd().set_non_blocking();//non-blocking connect
		LOG(INFO) << "trying to connect to: " << *(remote_addr.get_address_string().get()) << ":" << remote_addr.get_port_number();
		int ret = ::connect(new_stream.get_handle(), 
							reinterpret_cast<sockaddr*>(remote_addr.get_sockaddr_in_ptr().get()),
							remote_addr.get_size());
		return connect_error_handling(new_stream, ret, timeout);//timeout used to wait for the connect 
	}
    
protected:
//using sock_stream, so the type is sock_type::stream
	int shared_open(sock_stream& new_stream, int family,
		int protocol, int reuse_addr){
		if(!new_stream.has_handle()){
			if(new_stream.open_sock_fd(family, sock_type::stream, protocol, reuse_addr) == -1)
				return -1;
		}else{
			new_stream.close();
			if(new_stream.open_sock_fd(family, sock_type::stream, protocol, reuse_addr) == -1)
				return -1;
		}
		return 0;
	}

private:
	int connect_error_handling(sock_stream& new_stream, int ret, const micro_seconds& timeout){
		//if timeout == 0, means that we are not using non-blocking mode
		if(ret == -1 && timeout.count() != 0){
			//because we use non-blocking connect, so it will posibilly be EWOULDBLOCK
			if(errno == EWOULDBLOCK || errno == EINPROGRESS){
				LOG(ERROR) << strerror(errno);
				if(complete(new_stream, timeout) == -1){
					return -1;
				}
			}else{
				LOG(ERROR) << strerror(errno);
				return -1;
			}
		}
		return 0;
	}

	int complete(sock_stream& new_stream, const micro_seconds& timeout){
		auto timeout_milli_seconds = std::chrono::duration_cast<milliseconds>(timeout);
		// int h = unp::handle_timed_complete_using_poll(new_stream.get_handle(), 
		// 	timeout_milli_seconds);
		// if(h == INVALID_HANDLER) {
		// 	new_stream.close();
		// 	return -1;
		// }
		new_stream.get_sock_fd().restore_blocking(); //you can read now
		return 0;
	}
};
}

#endif // _UNP_SOCK_CONNECTOR_H_