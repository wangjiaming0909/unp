#include "server/reactor/connector.h"

using namespace reactor;

connector::~connector()
{

}

int connector::handle_input(int)
{

}

int connector::connect(const net::inet_addr& target_addr, const micro_seconds& timeout)
{
    int handle = make_connection_handler();
    if(handle < 0) return -1;
    net::sock_stream& stream = connection_handlers_[handle]->get_sock_stream();

   if(connector_.connect(stream, target_addr, &timeout, 1, 0) != 0)
   {
        LOG(WARNING) << "connect to " << target_addr.get_address_string() << " error...";
        connection_handlers_.erase(handle);
        return -1;
   }

   return activate_connection_handler(handle);
}

int connector::make_connection_handler()
{
    if(connection_handlers_.size() >= INT32_MAX)
    {
        LOG(WARNING) << "Too many connectors: " << connection_handlers_.size();
        return -1;
    }
    auto handler_ptr = std::make_shared<connection_handler>(*reactor_);
    connection_handlers_[handler_ptr->get_handle()] = handler_ptr;
    return handler_ptr->get_handle();
}

int connector::activate_connection_handler(int handle)
{
    return connection_handlers_[handle]->open();
}
