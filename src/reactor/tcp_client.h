#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include "net/inet_addr.h"
#include "net/unp.h"
#include "reactor/connector.h"
#include <chrono>

class Reactor;

namespace reactor
{

class tcp_client
{
public:
    using reactor_ptr_t = std::shared_ptr<Reactor>;
    using microseconds_t = std::chrono::microseconds;
    tcp_client();
    ~tcp_client();
    int open(unp::reactor_imp_t_enum type);
    int connect(const net::inet_addr& target_addr_, const microseconds_t& timeout);

private:
    reactor_ptr_t make_reactor(unp::reactor_imp_t_enum type);

private:
    reactor_ptr_t       reactor_;
};

} //namespace reactor
#endif // TCP_CLIENT_H
