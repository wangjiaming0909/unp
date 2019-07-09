#include "HHWheelTimer.h"

namespace reactor{

HHWheelTimer::HHWheelTimer(Reactor& reactor, time_t interval, time_t defaultTimeout) 
    : interval_(interval)
    , defaultTimeout_(defaultTimeout)
    , timerCount_(0)
    , startTime_(getCurTime())
    , handlers_()
    , reactor_(&reactor)
{
}

HHWheelTimer::HHWheelTimer( time_t interval , time_t defaultTimeout )
    : interval_(interval)
    , defaultTimeout_(defaultTimeout)
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
    auto baseTick = tickOfCurTime();
    scheduleTimeoutImpl_(handler, baseTick, timeout);
    handler.setSheduled(this, timeout);
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
void HHWheelTimer::scheduleTimeoutImpl_(TimeoutHandler& handler, int64_t baseTick, time_t timeout)
{
    auto tickOfTimeout = getTickFromDuration(timeout);
    auto ticksToGo = tickOfTimeout - baseTick;
    intrusive_list_t *pos;
    /**
     * scheduleTimeout时可能有几种情况:
     * 1, 可能schedule的timeout正好在当前时间所在的tick上, 也就是所传递的timeout与curTime在同一个interval中
     */

    if(ticksToGo < 0)//it means that the timeout has already timed out, so do nothing
    {
        return;
    }else if(ticksToGo < WHEEL_SIZE)
    {
        pos = &handlers_[0][ticksToGo];
        handler.posInBucket = ticksToGo;
    }
    else if(ticksToGo < (1 << 2 * WHEEL_SIZE))
    {
        pos = &handlers_[1][ticksToGo >> (WHEEL_BITS)];
    }
    else if(ticksToGo < (1 << (3 * WHEEL_SIZE)))
    {
        pos = &handlers_[2][ticksToGo >> (2 * WHEEL_BITS)];
    }
    else //currently, the buckets can't take this timeout, cause the time is very long time later
    {
        if(ticksToGo > LARGEST_SLOT)
        {
            ticksToGo = LARGEST_SLOT;
        }
        pos = &handlers_[3][ticksToGo >> (3 * WHEEL_BITS)];
    }
    pos->push_back(handler);
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
    expireTick_ = handler.posInBucket;

    // reactor_->register_handler();
}

inline int64_t  HHWheelTimer::tickOfCurTime() const
{
    return (getCurTime() - startTime_) / interval_;
}

} // reactor