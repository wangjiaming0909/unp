#include "HHWheelTimer.h"

namespace reactor{

int HHWheelTimer::DEFAULT_TICK_INTERVAL = 10;

HHWheelTimer::HHWheelTimer(Reactor& reactor, time_t interval, time_t defaultTimeout) 
    : interval_(interval)
    , defaultTimeout_(defaultTimeout)
    , currentTick_(1)
    , timerCount_(0)
    , startTime_(getCurTime())
    , handlers_()
    , reactor_(&reactor)
{
}

HHWheelTimer::HHWheelTimer( time_t interval , time_t defaultTimeout )
    : interval_(interval)
    , defaultTimeout_(defaultTimeout)
    , currentTick_(1)
    , timerCount_(0)
    , startTime_(getCurTime())
    , handlers_()
    , reactor_(nullptr)
{ 
}

HHWheelTimer::~HHWheelTimer()
{
}

void HHWheelTimer::scheduleTimeout(TimeoutHandler &handler, time_t timeout)
{
    scheduleTimeoutImpl_(timeout);
    scheduleInReactor_(handler);
}

void HHWheelTimer::timeoutExpired() noexcept 
{

}
size_t HHWheelTimer::cancelTimeoutsFromList(intrusive_list_t& handlers)
{

}

void HHWheelTimer::scheduleTimeoutImpl_(time_t timeout)
{

}

void HHWheelTimer::scheduleInReactor_(TimeoutHandler& handler)
{
    if(!handler.isAttachedToReactor() && !isScheduled())
    {
        LOG(WARNING) << "No reactor is attached to the timeout handler";
        return;
    }

    if(handler.isAttachedToReactor() && isScheduled())
    {
        assert(handler.get_reactor() == reactor_);
    }

    if(handler.isAttachedToReactor()) reactor_ = handler.get_reactor();
    else handler.setReactor(*reactor_);

    // reactor_->register_handler();
}

} // reactor