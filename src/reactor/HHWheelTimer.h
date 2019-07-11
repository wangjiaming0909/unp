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
    void timeoutExpired() noexcept ;
    size_t cancelTimeoutsFromList(intrusive_list_t& handlers);
    bool isScheduled() const {return reactor_ != nullptr;}
    size_t getTimerCount() const {return timerCount_;}

protected:
    virtual ~HHWheelTimer();
private:
    //find the right timeout and register the handlers into the reactor
    void scheduleNextTimeoutInReactor_(int64_t baseTick);
    // find the right place to put the timeout
    void scheduleTimeoutImpl_(TimeoutHandler& handler, int64_t baseTick, int64_t thisTimerExpireTick);
    int64_t getTickFromDuration(time_t duration) { return duration.count() / interval_.count(); }
    int64_t tickOfCurTime() const;

private:
    time_t interval_; // the interval of one tick
    time_t defaultTimeout_;
    size_t timerCount_;
    time_point_t startTime_;
    int64_t expireTick_{0};

    static constexpr int WHEEL_BUCKETS = 4;
    static constexpr int WHEEL_BITS = 8;
    static constexpr unsigned int WHEEL_SIZE = (1 << WHEEL_BITS);
    // static constexpr int DEFAULT_TICK_INTERVAL = 10;
    // static constexpr unsigned int WHEEL_MASK = (WHEEL_SIZE - 1);
    static constexpr uint32_t LARGEST_SLOT = 0xffffffffUL;

    intrusive_list_t handlers_[WHEEL_BUCKETS][WHEEL_SIZE];
    Reactor* reactor_;

    util::min_heap<uint8_t> registeredSlotsInFirstbucket_;
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
            wheel_->timeoutExpired();
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