/*************************************************************************
	> File Name: Client.h
	> Author: 
	> Mail: 
	> Created Time: Fri 28 Sep 2018 09:04:23 AM CST
 ************************************************************************/

#ifndef _CLIENT_H
#define _CLIENT_H
#include <netinet/in.h>

namespace server {
class Server;
}
namespace client{

class Client{
    friend class server::Server;
public:
    Client();
    ~Client();

private:
    sockaddr_in 	m_addr;
    socklen_t		m_sock_len;
};
#endif

}
