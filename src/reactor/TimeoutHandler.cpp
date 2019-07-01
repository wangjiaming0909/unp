#include "reactor/TimeoutHandler.h"

namespace reactor
{

TimeoutHandler::TimeoutHandler(Reactor& reactor) : event_handler(reactor)
{
}

TimeoutHandler::~TimeoutHandler()
{
}



}