#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include "net/inet_addr.h"
#include "net/unp.h"
#include "reactor/connector.h"
#include "boost/noncopyable.hpp"
#include <chrono>

class Reactor;

namespace reactor
{

class tcp_client : boost::noncopyable
{
public:
    using reactor_ptr_t = std::shared_ptr<Reactor>;
    using microseconds_t = std::chrono::microseconds;
    tcp_client();
    ~tcp_client();
    int open(unp::reactor_imp_t_enum type);
    void addConnector(IConnector& connector);
    void closeConnector(IConnector& connector);
    int start();
    int suspend();
    int stop();

private:
    reactor_ptr_t make_reactor(unp::reactor_imp_t_enum type);

private:
    std::vector<IConnector*> connectors_;

    reactor_ptr_t       reactor_;
};

} //namespace reactor
#endif // TCP_CLIENT_H
