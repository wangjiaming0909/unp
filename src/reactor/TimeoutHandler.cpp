#include "reactor/TimeoutHandler.h"

namespace reactor
{

TimeoutHandler::TimeoutHandler(Reactor& reactor) : EventHandler(reactor)
{
}

TimeoutHandler::~TimeoutHandler()
{
}



}