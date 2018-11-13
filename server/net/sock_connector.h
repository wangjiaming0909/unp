#ifndef _UNP_SOCK_CONNECTOR_H_
#define _UNP_SOCK_CONNECTOR_H_

#include "sock_stream.h"

namespace net{

class sock_connector{
public:
    sock_connector(){}
    ~sock_connector(){}
    
protected:
	int shared_open(sock_stream& new_stream, sock_type type,
		int protocol, int reuse_addr){
		if(new_stream.has_handle() 
			&& new_stream.open_sock_fd(type, protocol, reuse_addr) == -1)
			return -1;
		else return 0;
	}

private:


};
}

#endif // _UNP_SOCK_CONNECTOR_H_