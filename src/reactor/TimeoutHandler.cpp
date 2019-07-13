#include "reactor/TimeoutHandler.h"
#include "reactor/reactor.h"

namespace reactor
{

TimeoutHandler::TimeoutHandler(Reactor& reactor) : EventHandler(reactor)
{
}

TimeoutHandler::~TimeoutHandler()
{
}


void TimeoutHandler::setSheduled(HHWheelTimer* wheel, const TimePoint_T& timeout)
{
    assert(wheel != nullptr);
    wheel_ = wheel;
    expiration_ = timeout;
}

inline bool TimeoutHandler::isRegistered() const
{
    return reactor_->hasEvent(EventHandler::TIMEOUT_EVENT);
}

}