#include "reactor/connector.h"
#include "reactor/echo_client_handler.h"

using namespace reactor;

connector::~connector()
{

}

int connector::handle_input(int)
{
	return 0;
}

int connector::connect(const net::inet_addr& target_addr, const micro_seconds& timeout)
{
    auto handler = make_connection_handler();
    if(!handler) return -1;

    net::sock_stream &stream = handler->get_sock_stream();

    if (connector_.connect(stream, target_addr, &timeout, 1, 0) != 0)
    {
        LOG(WARNING) << "connect to " << target_addr.get_address_string() << " error...";
        return -1;
   }

   auto handle = handler->get_handle();
   connection_handlers_[handle] = handler;

   int ret = activate_connection_handler(handle);
   if(ret != 0)
    {
        LOG(WARNING) << "activate connection handler error";
        connection_handlers_[handle].reset();
        return -1;
    }
    return handle;
}

connector::connection_handler_ptr_t connector::make_connection_handler()
{
    if(connection_handlers_.size() >= INT32_MAX)
    {
        LOG(WARNING) << "Too many connectors: " << connection_handlers_.size();
        return 0;
    }
    return std::make_shared<echo_client_handler>(*reactor_);
}

int connector::activate_connection_handler(int handle)
{
    return connection_handlers_[handle]->open();
}
