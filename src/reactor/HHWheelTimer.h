#ifndef _UTIL_HHWHEELTIMER_H_
#define _UTIL_HHWHEELTIMER_H_
#include <stdint.h>
#include <chrono>
#include <cassert>
#include "reactor/TimeoutHandler.h"
#include "boost/intrusive/list.hpp"
#include "util/easylogging++.h"
#include "reactor/reactor.h"
#include "util/min_heap.h"

#define DEFAULT_TICK_INTERVAL 10

namespace reactor
{

struct Slot
{
    using SlotSize_t = uint8_t;
    static Slot NotFoundSlot;
    size_t bucketIndex;
    SlotSize_t slotIndex;

    bool operator==(const Slot &other) const { return bucketIndex == other.bucketIndex && slotIndex == other.slotIndex; }
};

class Buckets{
public:
    using SlotSize_t = Slot::SlotSize_t;
    using MinHeap_t = util::min_heap<SlotSize_t>;
    template <typename T>
    using Vector_t = std::vector<T>;

    Buckets(size_t size) noexcept: countOfSlotSet_{0}, buckets_{}, unsetBuckets_{}
    {
        buckets_.resize(size);
        initializeUnsetBuckets(size);
    }

    void initializeUnsetBuckets(size_t bucketSize);
    bool setRegistered(size_t bucket, SlotSize_t slot);
    bool unsetRegistered(size_t bucket, SlotSize_t slot);
    Slot findFirstSlot();
    Slot findFirstSlotInFirstBucket();
    int64_t count() const {return countOfSlotSet_;}

private:
#ifdef TESTING
public:
#endif
    int64_t countOfSlotSet_;
    Vector_t<MinHeap_t> buckets_;
    Vector_t<Vector_t<uint8_t>> unsetBuckets_;
};
/**
     * One HHWheelTimer only should be put in one reactor.
     * If scheduleTimeouts in different reactor, there'll have race conditions.
     *
     * Questions:
     * 1, Can we schedule timeouts in different reactors(EventBases)? Will it be a problem?
     */

class HHWheelTimer : boost::noncopyable
{
public:
    using time_t = std::chrono::milliseconds;
    using time_point_t = std::chrono::steady_clock::time_point;
    using intrusive_list_t = boost::intrusive::list<TimeoutHandler, boost::intrusive::constant_time_size<false>>;

    HHWheelTimer(
        time_t interval = time_t(DEFAULT_TICK_INTERVAL),
        time_t defaultTimeout = time_t(-1));
    HHWheelTimer(
        Reactor& reactor,
        time_t interval = time_t(DEFAULT_TICK_INTERVAL),
        time_t defaultTimeout = time_t(-1));
    time_t getDefaultTimeout() const { return defaultTimeout_; }
    time_t getInterval() const { return interval_; }
    void setDefaultTimeout(time_t &defaultTimeout) { defaultTimeout_ = defaultTimeout; }
    template <typename Fn>
    void scheduleTimeoutFn(Fn f, time_t timeout);
    void scheduleTimeout(TimeoutHandler &handler, time_t timeout);
    void timeoutExpired(TimeoutHandler* handler) noexcept ;
    size_t cancelTimeoutsFromList(intrusive_list_t& handlers);
    bool isScheduled() const {return reactor_ != nullptr;}
    size_t getTimerCount() const {return timerCount_;}

protected:
    virtual ~HHWheelTimer();
private:
#ifdef TESTING
public:
#endif
    //find the right timeout and register the handlers into the reactor
    void scheduleNextTimeoutInReactor_(int64_t baseTick, int64_t thisTimerExpireTick);
    // find the right place to put the timeout
    void scheduleTimeoutImpl_(TimeoutHandler& handler, int64_t baseTick, int64_t thisTimerExpireTick);
    template<typename Duration>
    int64_t getTickFromDuration(Duration duration)
    {
        return (std::chrono::duration_cast<time_t>(duration)).count() / interval_.count();
    }
    int64_t tickOfCurTime(const time_point_t& curTime) const;
    int64_t tickOfCurTime() const;
    time_point_t curTime() const;

private:
#ifdef TESTING
public:
#endif
    time_t interval_; // the interval of one tick
    time_t defaultTimeout_;
    size_t timerCount_;
    time_point_t startTime_;
    int64_t expireTick_{INT64_MAX};

    static constexpr int WHEEL_BUCKETS = 4;
    static constexpr int WHEEL_BITS = 8;
    static constexpr unsigned int WHEEL_SIZE = (1 << WHEEL_BITS);
    // static constexpr int DEFAULT_TICK_INTERVAL = 10;
    // static constexpr unsigned int WHEEL_MASK = (WHEEL_SIZE - 1);
    static constexpr uint32_t LARGEST_SLOT = 0xffffffffUL;

    intrusive_list_t handlers_[WHEEL_BUCKETS][WHEEL_SIZE];
    Reactor* reactor_;

    Buckets registeredBucketsSlots_;
};

template <typename Fn>
void HHWheelTimer::scheduleTimeoutFn(Fn f, time_t timeout)
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

HHWheelTimer::time_point_t getCurTime();

}//reactor
#endif // _UTIL_HHWHEELTIMER_H_