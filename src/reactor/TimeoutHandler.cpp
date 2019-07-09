#include "reactor/TimeoutHandler.h"

namespace reactor
{

TimeoutHandler::TimeoutHandler(Reactor& reactor) : EventHandler(reactor)
{
}

TimeoutHandler::~TimeoutHandler()
{
}


void TimeoutHandler::setSheduled(HHWheelTimer* wheel, std::chrono::microseconds timeout)
{
    assert(wheel != nullptr);
    wheel_ = wheel;
    expiration_ = std::chrono::steady_clock::now() + timeout;
}

}