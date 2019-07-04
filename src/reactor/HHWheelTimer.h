#ifndef _UTIL_HHWHEELTIMER_H_
#define _UTIL_HHWHEELTIMER_H_
#include <stdint.h>
#include <chrono>
#include <cassert>
#include "reactor/TimeoutHandler.h"
#include "boost/intrusive/list.hpp"
#include "util/easylogging++.h"

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
    void scheduleTimeout(Fn f, time_t timeout);
    void scheduleTimeout(TimeoutHandler &handler, time_t timeout);
    void timeoutExpired() noexcept ;
    size_t cancelTimeoutsFromList(intrusive_list_t& handlers);
    bool isScheduled() const {return reactor_ != nullptr;}

protected:
    virtual ~HHWheelTimer();
private:
    // register the handlers in the first List of first bucket
    void scheduleInReactor_(TimeoutHandler& handler);
    void scheduleTimeoutImpl_(time_t timeout);

private:
    time_t interval_; // the interval of one tick
    time_t defaultTimeout_;
    int64_t currentTick_;
    size_t timerCount_;
    time_point_t startTime_;

    static constexpr int WHEEL_BUCKETS = 4;
    static constexpr int WHEEL_BITS = 8;
    static constexpr unsigned int WHEEL_SIZE = (1 << WHEEL_BITS);
    static int DEFAULT_TICK_INTERVAL;
    // static constexpr unsigned int WHEEL_MASK = (WHEEL_SIZE - 1);
    // static constexpr uint32_t LARGEST_SLOT = 0xffffffffUL;

    intrusive_list_t handlers_[WHEEL_BUCKETS][WHEEL_SIZE];
    Reactor* reactor_;
};

template <typename Fn>
void HHWheelTimer::scheduleTimeout(Fn f, time_t timeout)
{
    struct TimeoutHandlerWrapper : public TimeoutHandler
    {
        TimeoutHandlerWrapper(Fn f) 
            : TimeoutHandler(), fn_(std::move(f)){}
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
            delete this;
        }
        Fn fn_;
    };

    TimeoutHandlerWrapper *handler = new TimeoutHandlerWrapper(f);
    scheduleTimeout(*handler, timeout);
}

HHWheelTimer::time_point_t getCurTime()
{
    return std::chrono::steady_clock::now();
}

}//reactor
#endif // _UTIL_HHWHEELTIMER_H_