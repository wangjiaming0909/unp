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

std::chrono::steady_clock::time_point getCurTime();

class HHWheelTimer : boost::noncopyable
{
public:
    using Duration = std::chrono::milliseconds;
    using TimeoutHandler_t = TimeoutHandler;
    using time_point_t = std::chrono::steady_clock::time_point;
    using intrusive_list_t = boost::intrusive::list<TimeoutHandler_t, boost::intrusive::constant_time_size<false>>;

    HHWheelTimer(
        Reactor* reactor = nullptr,
        Duration interval = Duration(DEFAULT_TICK_INTERVAL),
        Duration defaultTimeout = Duration(-1));
    Duration getDefaultTimeout() const { return defaultTimeout_; }
    Duration getInterval() const { return interval_; }
    void setDefaultTimeout(Duration &defaultTimeout) { defaultTimeout_ = defaultTimeout; }
    template <typename Fn>
    void scheduleTimeoutFn(Fn f, Duration timeout);
    void scheduleTimeout(TimeoutHandler_t &handler, Duration timeout);
    void timeoutExpired(TimeoutHandler_t* handler) noexcept ;
    size_t getTimerCount() const {return timerCount_;}
    /**
     * how to cancel timeouts
     * 1, it's possible that we've registered more than one timeout handlers into the reactor
     *   so, do we need to cancel the timeouts that already scheduled into the reactor
     *   acorroding to folly, only when there is no timeouts in the HHWheelTimer, then  
     *   the timeout event will be deleted in the EventBase, so here we only delete the 
     *   timeouts that are in the HHWheelTimer, for those already registered into the reactor,
     *   we try to disable it's timeoutCallback
     *  2, so, how to cancel a timeout in the HHWheelTimer?
     *      first, we need to reset the bit in the bitmap,
     *      unlink it from the intrusiveHandlersList,
     *          if the handler is scheduled through scheduleTimeoutFn(), 
     *          (the TimeoutHandler is newed, so we need to delete it)
     *          delete the TimeoutHandler
     *  3, If there is no timeouts in the HHWheelTimer, cancel timeoutEvent from the Reactor
     */
    void cancelAll();

// protected:
    virtual ~HHWheelTimer();
private:
#ifdef TESTING
public:
#endif
    size_t cancelTimeoutsFromList_(intrusive_list_t& handlers);
    //find the right timeout and register the handlers into the reactor
    void scheduleNextTimeoutInReactor_(TimeoutHandler_t *handler, int64_t baseTick, int64_t thisTimerExpireTick);
    // find the right place to put the timeout
    void scheduleTimeoutImpl_(TimeoutHandler_t& handler, int64_t baseTick, int64_t thisTimerExpireTick);
    int cascadeTimers_(int bucket, int tick);
    template<typename Duration2>
    int64_t getTickFromDuration(Duration2 duration)
    {
        return (std::chrono::duration_cast<Duration>(duration)).count() / interval_.count();
    }
    int64_t tickOfCurTime(const time_point_t& curTime) const;
    int64_t tickOfCurTime() const;
    time_point_t getCurTime() const {return reactor::getCurTime();}

private:
#ifdef TESTING
public:
#endif
    Reactor *reactor_;
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

    // Buckets registeredBucketsSlots_;
    std::bitset<WHEEL_SIZE> firstBucketBitSet_;
};

template <typename Fn>
void HHWheelTimer::scheduleTimeoutFn(Fn f, Duration timeout)
{
    auto *handler = new TimeoutHandler_t(true);
    handler->timeoutCallback = f;
    scheduleTimeout(*handler, timeout);
}

}//reactor
#endif // _UTIL_HHWHEELTIMER_H_