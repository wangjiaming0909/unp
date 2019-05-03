#include "tcp_server.h"

namespace reactor
{

tcp_server::tcp_server(const net::inet_addr& local_addr)
    : reactor_()
    , acceptor_()
    , pool_()
    , local_addr_(local_addr)
{
}

int tcp_server::open()
{
    
}

int tcp_server::close()
{

}





} //namespace reactor
