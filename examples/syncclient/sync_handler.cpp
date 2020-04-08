#include "sync_handler.h"
#include "reactor/reactor.h"

namespace filesync
{


  SyncHandler::SyncHandler(reactor::Reactor& react)
    : connection_handler(react)
  {}

  SyncHandler::~SyncHandler()
  {}


}
