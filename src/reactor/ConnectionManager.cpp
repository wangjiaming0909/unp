#include "reactor/ConnectionManager.h"

namespace reactor
{

ConnectionManager::ConnectionManager(Reactor& react) : reactor_(react)
{
}

ConnectionManager::~ConnectionManager()
{
}
}