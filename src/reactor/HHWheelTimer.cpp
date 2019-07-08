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
    if(!isScheduled()) return;//add timeouts into the HHWheelTimer and schedule it later(maybe)
    scheduleInReactor_(handler);
}

void HHWheelTimer::timeoutExpired() noexcept 
{

}
size_t HHWheelTimer::cancelTimeoutsFromList(intrusive_list_t& handlers)
{

}

/**
 * schedule timeout时, 当前timeout所放的位置与什么有关?
 * 1, 与当前时间所在的tick有关 
 * 2, 与所指定的timeout有关
 * */
void HHWheelTimer::scheduleTimeoutImpl_(time_t timeout)
{
    auto tickOfTimeout = getTickFromDuration(timeout);
    /**
     * scheduleTimeout时可能有几种情况:
     * 1, 可能schedule的timeout正好在当前时间所在的tick上, 也就是所传递的timeout与curTime在同一个interval中
     */

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

inline int64_t  HHWheelTimer::tickOfCurTime() const
{
    return (getCurTime() - startTime_) / interval_;
}

} // reactor