#include "reactor/TimeoutHandler.h"
#include "reactor/reactor.h"
#include "reactor/HHWheelTimer.h"

namespace reactor
{

TimeoutHandler::TimeoutHandler(Reactor& reactor) : EventHandler(reactor)
{
}

TimeoutHandler::~TimeoutHandler()
{
}


int TimeoutHandler::handle_timeout(int) noexcept
{
    try
    {
        if(timeoutCallback) timeoutCallback(this);
    }
    catch(const std::exception& e)
    {

    } 
    catch(...)
    {

    }

    wheel_->timeoutExpired(this);
}


}