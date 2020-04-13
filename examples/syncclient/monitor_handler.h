#pragma once
#include "reactor/connection_handler.h"
#include "reactor/reactor.h"

namespace filesync
{


class FileMonitorHandler : public reactor::connection_handler
{
public:
    FileMonitorHandler(reactor::Reactor& react);
};

class ServerMonitorHandler : public reactor::connection_handler
{
public:
enum class ServerStatus{idle, connected, disconnected, reconnecting};
public:
    ServerMonitorHandler(reactor::Reactor& react);

public:
    virtual int handle_input(int handle) override;
    ServerStatus getServerStatus() const {return serverStatus_;}

private:
    int sayHello();
    ServerStatus serverStatus_;
};

}