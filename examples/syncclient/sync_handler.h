#pragma once
#include "reactor/connection_handler.h"
#include "reactor/reactor.h"
#include "reactor/TimeoutHandler.h"

namespace filesync
{

class SyncHandler : public reactor::connection_handler
{
  public:
    SyncHandler(reactor::Reactor& react);
    ~SyncHandler();

};
}
