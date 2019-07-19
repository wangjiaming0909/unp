#ifndef _UTIL_HHWHEELTIMER_H_
#define _UTIL_HHWHEELTIMER_H_
#include <stdint.h>
#include <chrono>
#include <cassert>
#include <bitset>
#include "reactor/TimeoutHandler.h"
#include "boost/intrusive/list.hpp"
#include "util/easylogging++.h"
#include "reactor/reactor.h"
#include "util/min_heap.h"

namespace reactor
{

template <typename Duration>
struct HHWHeelTimerDurationConst;

template <>
struct HHWHeelTimerDurationConst<std::chrono::milliseconds>
{
    static constexpr int DEFAULT_TICK_INTERVAL = 10;
};

template <>
struct HHWHeelTimerDurationConst<std::chrono::microseconds>
{
    static constexpr int DEFAULT_TICK_INTERVAL = 200;
};

// struct Slot
// {
//     using SlotSize_t = uint8_t;
//     static Slot NotFoundSlot;
//     size_t bucketIndex;
//     SlotSize_t slotIndex;

//     bool operator==(const Slot &other) const { return bucketIndex == other.bucketIndex && slotIndex == other.slotIndex; }
// };

// class Buckets{
// public:
//     using SlotSize_t = Slot::SlotSize_t;
//     using MinHeap_t = util::min_heap<SlotSize_t>;
//     template <typename T>
//     using Vector_t = std::vector<T>;

//     Buckets(size_t size) noexcept: countOfSlotSet_{0}, buckets_{}, unsetBuckets_{}
//     {
//         buckets_.resize(size);
//         initializeUnsetBuckets(size);
//     }

//     void initializeUnsetBuckets(size_t bucketSize);
//     bool setRegistered(size_t bucket, SlotSize_t slot);
//     bool unsetRegistered(size_t bucket, SlotSize_t slot);
//     Slot findFirstSlot();
//     Slot findFirstSlotInFirstBucket();
//     int64_t count() const {return countOfSlotSet_;}

// private:
// #ifdef TESTING
// public:
// #endif
//     int64_t countOfSlotSet_;
//     Vector_t<MinHeap_t> buckets_;
//     Vector_t<Vector_t<uint8_t>> unsetBuckets_;
// };
/**
     * One HHWheelTimer only should be put in one reactor.
     * If scheduleTimeouts in different reactor, there'll have race conditions.
     *
     * Questions:
     * 1, Can we schedule timeouts in different reactors(EventBases)? Will it be a problem?
     */

template <typename Duration>
class HHWheelTimer : boost::noncopyable
{
public:
    using time_point_t = std::chrono::steady_clock::time_point;
    using intrusive_list_t = boost::intrusive::list<TimeoutHandler, boost::intrusive::constant_time_size<false>>;

    HHWheelTimer(
        Duration interval = Duration(DEFAULT_TICK_INTERVAL),
        Duration defaultTimeout = Duration(-1));
    HHWheelTimer(
        Reactor& reactor,
        Duration interval = Duration(DEFAULT_TICK_INTERVAL),
        Duration defaultTimeout = Duration(-1));
    Duration getDefaultTimeout() const { return defaultTimeout_; }
    Duration getInterval() const { return interval_; }
    void setDefaultTimeout(Duration &defaultTimeout) { defaultTimeout_ = defaultTimeout; }
    template <typename Fn>
    void scheduleTimeoutFn(Fn f, Duration timeout);
    void scheduleTimeout(TimeoutHandler &handler, Duration timeout);
    void timeoutExpired(TimeoutHandler* handler) noexcept ;
    bool isScheduled() const {return reactor_ != nullptr;}
    size_t getTimerCount() const {return timerCount_;}
    void cancelAll();

protected:
    virtual ~HHWheelTimer();
private:
#ifdef TESTING
public:
#endif
    size_t cancelTimeoutsFromList(intrusive_list_t& handlers);
    //find the right timeout and register the handlers into the reactor
    void scheduleNextTimeoutInReactor_(int64_t baseTick, int64_t thisTimerExpireTick);
    // find the right place to put the timeout
    void scheduleTimeoutImpl_(TimeoutHandler& handler, int64_t baseTick, int64_t thisTimerExpireTick);
    template<typename Duration2>
    int64_t getTickFromDuration(Duration2 duration)
    {
        return (std::chrono::duration_cast<Duration>(duration)).count() / interval_.count();
    }
    int64_t tickOfCurTime(const time_point_t& curTime) const;
    int64_t tickOfCurTime() const;
    time_point_t curTime() const;

private:
#ifdef TESTING
public:
#endif
    Duration interval_; // the interval of one tick
    Duration defaultTimeout_;
    size_t timerCount_;
    time_point_t startTime_;
    int64_t expireTick_{INT64_MAX};

    static constexpr int WHEEL_BUCKETS = 4;
    static constexpr int WHEEL_BITS = 8;
    static constexpr unsigned int WHEEL_SIZE = (1 << WHEEL_BITS);
    static int DEFAULT_TICK_INTERVAL;
    static constexpr unsigned int WHEEL_MASK = (WHEEL_SIZE - 1);
    static constexpr uint32_t LARGEST_SLOT = 0xffffffffUL;

    intrusive_list_t handlers_[WHEEL_BUCKETS][WHEEL_SIZE];
    Reactor* reactor_;

    // Buckets registeredBucketsSlots_;
    std::bitset<WHEEL_SIZE> firstBucketBitSet_;
};
template <typename Duration>
typename HHWheelTimer<Duration>::time_point_t getCurTime()
{
    return std::chrono::steady_clock::now();
}

template <typename Duration>
HHWheelTimer<Duration>::HHWheelTimer( Duration interval, Duration defaultTimeout)
    : interval_(interval)
    , defaultTimeout_(defaultTimeout)
    , timerCount_(0)
    , startTime_(getCurTime())
    , handlers_()
    , reactor_(nullptr)
    // , registeredBucketsSlots_{WHEEL_BUCKETS}
    , firstBucket_(false)
{

}

template <typename Duration>
HHWheelTimer<Duration>::HHWheelTimer(
    Reactor& reactor,
    Duration interval,
    Duration defaultTimeout)
    : interval_(interval)
    , defaultTimeout_(defaultTimeout)
    , timerCount_(0)
    , startTime_(getCurTime())
    , handlers_()
    , reactor_(&reactor)
    // , registeredBucketsSlots_{WHEEL_BUCKETS}
    , firstBucket_(false)
{

}

template <typename Duration>
HHWheelTimer<Duration>::~HHWheelTimer()
{

}

template <typename Duration>
void HHWheelTimer<Duration>::scheduleTimeout( TimeoutHandler &handler, Duration timeout)
{
    timeout = std::max(timeout, Duration::zero());
    timerCount_++;

    auto now = getCurTime();
    auto currentTick = tickOfCurTime(now);
    handler.setSheduled(this, timeout);

    int64_t timeoutTicks = getTickFromDuration(timeout);
    int64_t thisTimerExpireTick = timeoutTicks + currentTick;
    scheduleTimeoutImpl_(handler, currentTick, thisTimerExpireTick);

    if(!reactor_->hasEvent(EventHandler::TIMEOUT_EVENT) || thisTimerExpireTick < expireTick_)
    {
        scheduleNextTimeoutInReactor_(currentTick, thisTimerExpireTick);
    }
}

template <typename Duration>
template <typename Fn>
void HHWheelTimer<Duration>::scheduleTimeoutFn(Fn f, Duration timeout)
{
    struct TimeoutHandlerWrapper : public TimeoutHandler
    {
        TimeoutHandlerWrapper(Fn f) : TimeoutHandler(), fn_(std::move(f)){}
        int handle_timeout(int) noexcept override
        {
            try{ fn_(); } //for noexcept
            catch(const std::exception& e)
            {
                LOG(WARNING) << "TimeoutHandlerWrapper throw an exception: " << e.what();
            }
            catch(...)
            {
                LOG(WARNING) << "TimeoutHandlerWrapper throw a unknow exception: ";
            }
            wheel_->timeoutExpired(this);
            delete this;
        }
        Fn fn_;
    };

    TimeoutHandlerWrapper *handler = new TimeoutHandlerWrapper(std::move(f));
    scheduleTimeout(*handler, timeout);
}

template <typename Duration>
void HHWheelTimer<Duration>::scheduleTimeoutImpl_(TimeoutHandler& handler, int64_t baseTick, int64_t thisTimerExpireTick)
{
    int64_t diff = thisTimerExpireTick - baseTick;
    intrusive_list_t *handlerList;

    if(diff < 0)
    {
        // handlerList = &handlers_[0][]
    }else if(diff < WHEEL_SIZE) {
        handlerList = &handlers_[0][thisTimerExpireTick & WHEEL_MASK];
        firstBucketBitSet_.set(thisTimerExpireTick);
        handler.bucket_ = thisTimerExpireTick & WHEEL_MASK;
    } else if(diff < 1 << (2 * WHEEL_BITS)){
        handlerList = &handlers_[1][(thisTimerExpireTick >> WHEEL_BITS) & WHEEL_MASK];
    } else if(diff < 1 << (3 * WHEEL_BITS)){
        handlerList = &handlers_[2][(thisTimerExpireTick >> 2 * WHEEL_BITS) & WHEEL_MASK];
    }else {

    }
    handlerList->push_back(handler);
}

template <typename Duration>
void HHWheelTimer<Duration>::scheduleNextTimeoutInReactor_(int64_t baseTick, int64_t thisTimerExpireTick)
{
    if(thisTimerExpireTick == -1)
    {
        auto it = firstBucketBitSet_._Find_first();
        if(it == firstBucketBitSet_.size())
        {
            thisTimerExpireTick = WHEEL_SIZE - baseTick & WHEEL_MASK;
        }else 
        {
            thisTimerExpireTick = it;
        }
    }

    reactor_->register_handler(&timeoutHandler, EventHandler::TIMEOUT_EVENT);
    expireTick_ = thisTimerExpireTick;
}

template <typename Duration>
void HHWheelTimer<Duration>::timeoutExpired(TimeoutHandler* handler) noexcept
{
    auto curTick = tickOfCurTime();

    while(expireTick_ < curTick)
    {
        int index = expireTick_ & WHEEL_MASK;

        firstBucketBitSet_.reset(index);
        expireTick_++;
    }

    scheduleNextTimeoutInReactor_(expireTick_, -1);
}

template <typename Duration>
inline int64_t  HHWheelTimer<Duration>::tickOfCurTime(const time_point_t& curTime) const
{
    return (curTime - startTime_) / interval_;
}

template <typename Duration>
inline int64_t HHWheelTimer<Duration>::tickOfCurTime() const
{
    return tickOfCurTime(curTime());
}

template <typename Duration>
inline HHWheelTimer<Duration>::time_point_t HHWheelTimer<Duration>::curTime() const
{
    return getCurTime();
}

template <typename Duration>
int HHWheelTimer<Duration>::DEFAULT_TICK_INTERVAL = HHWHeelTimerDurationConst<Duration>::DEFAULT_TICK_INTERVAL;

}//reactor
#endif // _UTIL_HHWHEELTIMER_H_