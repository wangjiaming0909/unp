#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include "server/net/inet_addr.h"
#include "server/net/unp.h"
#include "server/reactor/connector.h"
#include <chrono>

class Reactor;

namespace reactor
{

class tcp_client
{
public:
    using reactor_ptr_t = std::shared_ptr<Reactor>;
    using connector_ptr_t = std::shared_ptr<connector>;
    using microseconds_t = std::chrono::microseconds;
    tcp_client();
    ~tcp_client();
    int open(unp::reactor_imp_t_enum type);
    int connect(const net::inet_addr& target_addr_, const microseconds_t& timeout);

private:
    reactor_ptr_t make_reactor(unp::reactor_imp_t_enum type);

private:
    reactor_ptr_t       reactor_;
    connector_ptr_t     connector_;
};

} //namespace reactor
#endif // TCP_CLIENT_H
