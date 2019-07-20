#include "HHWheelTimer.h"
#include "reactor/reactor.h"

namespace reactor{

std::chrono::steady_clock::time_point getCurTime()
{
    return std::chrono::steady_clock::now();
}

HHWheelTimer::HHWheelTimer(
    Reactor *reactor,
    Duration interval,
    Duration defaultTimeout)
    : reactor_(reactor)
    , interval_(interval)
    , defaultTimeout_(defaultTimeout)
    , timerCount_(0)
    , startTime_(getCurTime())
    , handlers_()
    , firstBucketBitSet_(false)
{

}

HHWheelTimer::~HHWheelTimer()
{

}

void HHWheelTimer::cancelAll()
{

}

void HHWheelTimer::scheduleTimeout(TimeoutHandler_t &handler, Duration timeout)
{
    timeout = std::max(timeout, Duration::zero());
    auto now = getCurTime();
    auto currentTick = tickOfCurTime(now);
    handler.setSheduled(*this, now + timeout);

    int64_t timeoutTicks = getTickFromDuration(timeout);
    int64_t thisTimerExpireTick = timeoutTicks + currentTick;

    timerCount_++;
    LOG(INFO) << "--------count++: " << timerCount_ << " thisTimerExpireTick: " << thisTimerExpireTick << " expireTick: " << expireTick_;
    scheduleTimeoutImpl_(handler, currentTick, thisTimerExpireTick);

    if(!reactor_->hasEvent(EventHandler::TIMEOUT_EVENT) || thisTimerExpireTick <= expireTick_)
    {
        scheduleNextTimeoutInReactor_(&handler, currentTick, thisTimerExpireTick);
    }
}

void HHWheelTimer::scheduleTimeoutImpl_(TimeoutHandler_t& handler, int64_t baseTick, int64_t thisTimerExpireTick)
{
    int64_t diff = thisTimerExpireTick - baseTick;
    intrusive_list_t *handlerList;

    if(diff < 0)
    {
        handlerList = &handlers_[0][baseTick & WHEEL_MASK];
        firstBucketBitSet_.set(baseTick & WHEEL_MASK);
        handler.bucket_ = (baseTick & WHEEL_MASK);
        LOG(INFO) << "seting pos: " << (baseTick & WHEEL_MASK);
    }else if(diff < WHEEL_SIZE) {
        handlerList = &handlers_[0][thisTimerExpireTick & WHEEL_MASK];
        firstBucketBitSet_.set(thisTimerExpireTick & WHEEL_MASK);
        LOG(INFO) << "seting pos: " << (thisTimerExpireTick & WHEEL_MASK) << " thisTimerExpireTick: " << thisTimerExpireTick;
        handler.bucket_ = (thisTimerExpireTick & WHEEL_MASK);
    } else if(diff < 1 << (2 * WHEEL_BITS)){
        LOG(INFO) << "!!!!thisTimerExpireTick: " << thisTimerExpireTick;
        handlerList = &handlers_[1][(thisTimerExpireTick >> WHEEL_BITS) & WHEEL_MASK];
    } else if(diff < 1 << (3 * WHEEL_BITS)){
        LOG(INFO) << "!!!!thisTimerExpireTick: " << thisTimerExpireTick;
        handlerList = &handlers_[2][(thisTimerExpireTick >> 2 * WHEEL_BITS) & WHEEL_MASK];
    }else {
        LOG(INFO) << ".0.0.0.0.0.0.0.0.0.0.0.0";
    }
    handlerList->push_back(handler);
}

void HHWheelTimer::scheduleNextTimeoutInReactor_(TimeoutHandler_t *handler, int64_t baseTick, int64_t thisTimerExpireTick)
{
    if(thisTimerExpireTick == -1)
    {
        auto it = firstBucketBitSet_._Find_first();
        if(it == firstBucketBitSet_.size())
        {
            int64_t tickToGo = WHEEL_SIZE - (baseTick & WHEEL_MASK);
            thisTimerExpireTick = baseTick + tickToGo;
            LOG(INFO) << "setting expireTick: " << expireTick_;
            // scheduleTimeoutFn([=](TimeoutHandler*){LOG(INFO) << "|||||||||||||||||||||expired...";}, thisTimerExpireTick * interval_);
            handler = new TimeoutHandler_t(*reactor_);
            handler->timeoutCallback = [](TimeoutHandler_t*){LOG(INFO) << "|||||||||||||||||||||expired...";};
            handler->setSheduled(*this, startTime_ + thisTimerExpireTick * interval_);
        }else 
        {
            handler = &handlers_[0][it].front();
            thisTimerExpireTick = (handler->expiration_ - startTime_) / interval_;
        }
    }

    if(!handler->isRegistered)
    {
        reactor_->register_handler(handler, EventHandler::TIMEOUT_EVENT);
    }
    expireTick_ = thisTimerExpireTick;
    LOG(INFO) << "setting expireTick: " << expireTick_;
}

void HHWheelTimer::timeoutExpired(TimeoutHandler_t* handler) noexcept
{
    auto curTick = tickOfCurTime();

    intrusive_list_t expiredTimers;
    int index = expireTick_ & WHEEL_MASK;
    if(firstBucketBitSet_.test(index)) timerCount_--;
    LOG(INFO) << "-----------timer count--: " << timerCount_ << " expireTick: " << expireTick_;
    firstBucketBitSet_.reset(index);
    LOG(INFO) << "reseting pos: " << index;

    //if there are lots of timer that are expired at the curTick
    //we get them out and handle it
    LOG(INFO) << "expireTick: " << expireTick_ << " curTick: " << curTick;
    while(expireTick_ <= curTick)
    {
        index = expireTick_ & WHEEL_MASK;
        if(index == 0 && timerCount_ > 0)
        {
            if (cascadeTimers(1, (expireTick_ >> WHEEL_BITS) & WHEEL_MASK) &&
                cascadeTimers(2, (expireTick_ >> (2 * WHEEL_BITS)) & WHEEL_MASK)) {
                cascadeTimers(3, (expireTick_ >> (3 * WHEEL_BITS)) & WHEEL_MASK);
            }
        }
        expireTick_++;
        if(!firstBucketBitSet_.test(index)) continue;

        intrusive_list_t tmpList;
        tmpList.swap(handlers_[0][index]);

        while(!tmpList.empty())
        {
            auto& callback = tmpList.front();
            tmpList.pop_front();

            if(&callback == handler || handler->isRegistered) continue;
            expiredTimers.push_back(callback);
            timerCount_--;
            LOG(INFO) << "-----------timer count--: " << timerCount_;;
        }
        firstBucketBitSet_.reset(index);
    }

    while(!expiredTimers.empty())
    {
        expiredTimers.front().handle_timeout(0);
        expiredTimers.pop_front();
    }

    if(timerCount_ > 0)
        scheduleNextTimeoutInReactor_(nullptr, curTick, -1);
}

int HHWheelTimer::cascadeTimers(int bucket, int tick)
{
    LOG(INFO) << "-------------------cascading timeouts bucket: " << bucket << " tick: " << tick;
    intrusive_list_t tmp;
    tmp.swap(handlers_[bucket][tick]);
    auto now = getCurTime();
    auto curTick = tickOfCurTime(now);

    while(!tmp.empty())
    {
        auto & callback = tmp.front();
        tmp.pop_front();
        auto newThisTimerExpireTick = (callback.expiration_ - startTime_) / interval_;
        scheduleTimeoutImpl_(callback, curTick, newThisTimerExpireTick);
    }
    return tick == 0;
}

inline int64_t  HHWheelTimer::tickOfCurTime(const time_point_t& curTime) const
{
    return (curTime - startTime_) / interval_;
}

inline int64_t HHWheelTimer::tickOfCurTime() const
{
    return tickOfCurTime(getCurTime());
}

int HHWheelTimer::DEFAULT_TICK_INTERVAL = HHWHeelTimerDurationConst<Duration>::DEFAULT_TICK_INTERVAL;
} // reactor