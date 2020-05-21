#include "sync_handler.h"
#include "reactor/reactor.h"

namespace filesync
{


  SyncHandler::SyncHandler(reactor::Reactor& react)
    : sock_connection_handler(react)
  {}

  SyncHandler::~SyncHandler()
  {}


}
