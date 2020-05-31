#include "reactor/ConnectionManager.h"

namespace reactor
{

ConnectionManager::ConnectionManager(Reactor& react)
  : reactor_(react)
  , connections_{}
{
}

ConnectionManager::~ConnectionManager()
{
  closeAllConnection();
}

int ConnectionManager::closeAllConnection()
{
  while(!connections_.empty()) {
    auto* conn = &connections_.front();
    connections_.pop_front();
    conn->close();
    delete conn;
  }
  return connectionCount_;
}
}
