#include "reactor/ConnectionManager.h"

namespace reactor
{

ConnectionManager::ConnectionManager(Reactor& react) : reactor_(react)
{
}

ConnectionManager::~ConnectionManager()
{
    auto it = handlersMap_.begin();
    for(; it != handlersMap_.end(); it++)
    {
        delete it->second;
        it->second = nullptr;
    }
}
}