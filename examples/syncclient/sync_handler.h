#pragma once
#include "reactor/sock_connection_handler.h"
#include "reactor/reactor.h"
#include "reactor/TimeoutHandler.h"

namespace filesync
{

class SyncHandler : public reactor::sock_connection_handler
{
  public:
    SyncHandler(reactor::Reactor& react);
    ~SyncHandler();

};
}
