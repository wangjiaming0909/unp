#include "reactor/connector.h"
#include "reactor/echo_client_handler.h"

namespace reactor
{


int connector::connect(const net::inet_addr& target_addr, micro_seconds timeout)
{
    net::sock_stream &stream = handler_.get_sock_stream();

    if (connector_.connect(stream, target_addr, &timeout, 1, 0) != 0)
    {
        LOG(WARNING) << "connect to " << target_addr.get_address_string() << " error...";
        return -1;
   }

   if(handler_.open() != 0)
    {
        LOG(WARNING) << "activate connection handler error";
        return -1;
    }
    return 0;
}

int connector::disconnect(micro_seconds timeout)
{
    handler_.close();
}

}