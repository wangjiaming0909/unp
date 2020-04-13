#pragma once
#include "reactor/connection_handler.h"
#include "reactor/reactor.h"


class FileMonitorHandler : public reactor::connection_handler
{
public:
    FileMonitorHandler(reactor::Reactor& react) : reactor::connection_handler(react){}
};

class ServerMonitorHandler : public reactor::connection_handler
{
public:
    ServerMonitorHandler(reactor::Reactor& react) : reactor::connection_handler(react){};

};
