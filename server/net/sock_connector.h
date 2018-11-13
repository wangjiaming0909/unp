#ifndef _UNP_SOCK_CONNECTOR_H_
#define _UNP_SOCK_CONNECTOR_H_

#include "sock_stream.h"

namespace net{

class sock_connector{
public:
    sock_connector(){}
    ~sock_connector(){}
    
protected:
	int shared_open(sock_stream& new_stream, 
		int family, int protocol, int reuse_addr){
		if(new_stream.has_handle()){
			
		}
	}

private:


};
}

#endif // _UNP_SOCK_CONNECTOR_H_