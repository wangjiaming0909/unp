#include "HHWheelTimer.h"
#include "reactor/reactor.h"

namespace reactor{


void Buckets::initializeUnsetBuckets(size_t bucketSize)
{
    unsetBuckets_.resize(bucketSize);
    for(auto &v : unsetBuckets_)
    {
        v.resize(1 << (sizeof(SlotSize_t) * 8));
    }
}

bool Buckets::setRegistered(size_t bucket, SlotSize_t slot)
{
    if(bucket > buckets_.size()) return false;
    buckets_[bucket].push(slot);
    return true;
}

bool Buckets::unsetRegistered(size_t bucket, SlotSize_t slot)
{
    if(bucket > buckets_.size()) return false;
    if(!buckets_[bucket].empty() && buckets_[bucket].top() == slot)
    {
        buckets_[bucket].pop();
        return true;
    }
    auto &value = unsetBuckets_[bucket][slot];
    if(value != 0) return false;
    value = 1; return true;
}

Slot Buckets::findFirst() 
{
    for (int i = 0; i < buckets_.size(); i++)
    {
        while(buckets_[i].size() > 0)
        {
            if(unsetBuckets_[i][buckets_[i].top()] == 0)
            {
                return {i, buckets_[i].top()};
            }
            buckets_[i].pop();
        }
    }
    return Slot::NotFoundSlot;
}

Slot Slot::NotFoundSlot = {SIZE_MAX, UINT8_MAX};

HHWheelTimer::HHWheelTimer(Reactor& reactor, time_t interval, time_t defaultTimeout) 
    : interval_(interval)
    , defaultTimeout_(defaultTimeout)
    , timerCount_(0)
    , startTime_(getCurTime())
    , handlers_()
    , reactor_(&reactor)
    , registeredSlotsInFirstbucket_{}
    , registeredBucketsSlots_{WHEEL_BUCKETS}
{
}

HHWheelTimer::HHWheelTimer( time_t interval , time_t defaultTimeout )
    : interval_(interval)
    , defaultTimeout_(defaultTimeout)
    , timerCount_(0)
    , startTime_(getCurTime())
    , handlers_()
    , reactor_(nullptr)
    , registeredSlotsInFirstbucket_{}
    , registeredBucketsSlots_{WHEEL_BUCKETS}
{ 
}

HHWheelTimer::~HHWheelTimer()
{
}

void HHWheelTimer::scheduleTimeout(TimeoutHandler &handler, time_t timeout)
{
    auto baseTick = tickOfCurTime();
    auto thisTimerExpireTick = getTickFromDuration(timeout);
    scheduleTimeoutImpl_(handler, baseTick, thisTimerExpireTick);
    handler.setSheduled(this, timeout);

    if(!isScheduled()) return;//add timeouts into the HHWheelTimer and schedule it later(maybe)

    // 不一定是当前的这个timeout 会被 放到reactor中
    scheduleNextTimeoutInReactor_(baseTick);
}

/**
 * A little different from HHWheelTimer in folly
 * When registering timeouts into the reactor, 
 * for folly, HHWheelTimer inherts from AsyncTimeout, so timeoutExpired method is the default callback of timeout event
 * but, for here, we have TimeoutHandler for handling events, so at default, the reactor (or EventBase) will invoke the handler's callback in the handler, not here
 * Here is to do some clean up thing for the HHWheelTimer
 * 
 * 
 * So what should we do when one timeout is expired
 * 1, remove the timeouts from the wheel
 * 2, cascade timeouts which are not in the first bucket
 */
void HHWheelTimer::timeoutExpired() noexcept 
{
    timerCount_--;
    auto curTick = tickOfCurTime();

}

size_t HHWheelTimer::cancelTimeoutsFromList(intrusive_list_t& handlers)
{

}

/**
 * schedule timeout时, 当前timeout所放的位置与什么有关?
 * 1, 与当前时间所在的tick有关 
 * 2, 与所指定的timeout有关
 * */
void HHWheelTimer::scheduleTimeoutImpl_(TimeoutHandler& handler, int64_t baseTick, int64_t thisTimerExpireTick)
{
    auto ticksToGo = thisTimerExpireTick - baseTick;
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
        registeredSlotsInFirstbucket_.push(static_cast<uint8_t>(ticksToGo));
        registeredBucketsSlots_.setRegistered(0, static_cast<uint8_t>(ticksToGo));
        handler.posInBucket = ticksToGo;
    }
    else if(ticksToGo < (1 << 2 * WHEEL_BITS))
    {
        pos = &handlers_[1][ticksToGo >> (WHEEL_BITS)];
        registeredBucketsSlots_.setRegistered(1, static_cast<uint8_t>(ticksToGo >> (WHEEL_BITS)));
    }
    else if(ticksToGo < (1 << (3 * WHEEL_BITS)))
    {
        pos = &handlers_[2][ticksToGo >> (2 * WHEEL_BITS)];
        registeredBucketsSlots_.setRegistered(2, static_cast<uint8_t>(ticksToGo >> (2 * WHEEL_BITS)));
    }
    else //currently, the buckets can't take this timeout, cause the time is very long time later
    {
        if(ticksToGo > LARGEST_SLOT)
        {
            ticksToGo = LARGEST_SLOT;
        }
        pos = &handlers_[3][ticksToGo >> (3 * WHEEL_BITS)];
        registeredBucketsSlots_.setRegistered(3, static_cast<uint8_t>(ticksToGo >> (3 * WHEEL_BITS)));
    }
    pos->push_back(handler);
    timerCount_++;
}

void HHWheelTimer::scheduleNextTimeoutInReactor_(int64_t baseTick)
{
    if(!isScheduled())
    {
        LOG(WARNING) << "No reactor is attached to the HHWheelTimer";
        return;
    }

    /** 
     * Everytime schedule a timeout in the wheel, sometimes we donot need to register it into the reactor
     * There are some situations:
     * 1, There is no timeout event registered into the reactor(EventBase)
     * 2, Already registered timeout events into the reactor, The timeout we are scheduling is sooner than the expire time of last timeout
     *  if we do not register this sooner timer into the reactor, we'll miss it
     * 3, Already registered timeout events into the reactor, and the timer we are scheduling is later than last timeout, 
     * we do not register it into the reactor, wait until the first timeout and invoke timeoutExpired, than schedule the soonest timeout
     */
    // we assert that the timeouts event in the reactor are registered by this wheel if reactor has timeout events

    // Find the timeouts that will expire first in the wheel
    if(timerCount_ == 0) return;
    //min heap 中没有timeout 说明第一个bucket中没有timeout, 是否需要cascade?
    if(registeredSlotsInFirstbucket_.size() == 0)
    {
        //!! 如果第一个bucket中没有timeout, 但是有其他更晚的timeout, 
        //那么我们 注册一个相对较晚的timeout 作为默认timeout并且有默认的handler, 
        //用于唤醒自己来cascade timeouts 并且注册到时候已经位于第一个bucket中的timeouts
        // baseTick = 
    }

    auto posInbucket = registeredSlotsInFirstbucket_.top();
    auto firstSlot = &handlers_[0][posInbucket];
    registeredSlotsInFirstbucket_.pop();

    if(!reactor_->hasEvent(EventHandler::TIMEOUT_EVENT) || expireTick_ >=  baseTick)
    {
        for(auto &timeoutHandler : *firstSlot)
        {
            reactor_->register_handler(&timeoutHandler, EventHandler::TIMEOUT_EVENT);
        }
    }
}

inline int64_t  HHWheelTimer::tickOfCurTime() const
{
    return (getCurTime() - startTime_) / interval_;
}

inline HHWheelTimer::time_point_t getCurTime() { return std::chrono::steady_clock::now(); }

} // reactor