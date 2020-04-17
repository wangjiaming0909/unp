#include "syncclient/monitor_handler.h"


namespace filesync
{

FileMonitorHandler::FileMonitorHandler(reactor::Reactor& react) 
    : reactor::connection_handler(react)
{

}

ServerMonitorHandler::ServerMonitorHandler(reactor::Reactor& react) 
    : reactor::connection_handler(react)
{
}

int ServerMonitorHandler::open()
{
    this->enable_reading();
    return sayHello();
}

int ServerMonitorHandler::handle_input(int handle)
{
    if (connection_handler::handle_input(handle) < 0) 
    {
        return -1;
    }
    int ret = 0;
    char response[16] = {};
    read(response, 16);
    if (std::strncmp(response, "ok", 16) == 0)
    {
        serverStatus_ = ServerStatus::connected;
        ret = 0;
    }
    else 
    {
        serverStatus_ = ServerStatus::disconnected;
        ret = -1;
    }
    return ret;
}

int ServerMonitorHandler::sayHello()
{
    const char* hello = "hello";
    return write(hello, strlen(hello));
}

}