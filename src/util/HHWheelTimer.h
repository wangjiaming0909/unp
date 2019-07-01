#ifndef _UTIL_HHWHEELTIMER_H_
#define _UTIL_HHWHEELTIMER_H_
#include <stdint.h>
#include <chrono>
#include "reactor/TimeoutHandler.h"

namespace util
{
class HHWheelTimer
{
public:
    using time_t = std::chrono::milliseconds;
    HHWheelTimer(/* args */);

    time_t getDefaultTimeout() const { return defaultTimeout_; }
    time_t getInterval() const { return interval_; }
    void setDefaultTimeout(time_t &defaultTimeout) { defaultTimeout_ = defaultTimeout; }
    template <typename Fn>
    void scheduleTimeout(reactor::Reactor& reactor, Fn f, time_t timeout);
    void scheduleTimeout(reactor::TimeoutHandler &handler, time_t timeout);

protected:
    virtual ~HHWheelTimer();
private:
    HHWheelTimer(const HHWheelTimer &) = delete;
    HHWheelTimer &operator=(const HHWheelTimer &) = delete;
private:
    time_t interval_;
    time_t defaultTimeout_;

    static constexpr int WHEEL_BUCKETS = 4;
    static constexpr int WHEEL_BITS = 8;
    static constexpr unsigned int WHEEL_SIZE = (1 << WHEEL_BITS);
    // static constexpr unsigned int WHEEL_MASK = (WHEEL_SIZE - 1);
    // static constexpr uint32_t LARGEST_SLOT = 0xffffffffUL;
};

template <typename Fn>
void HHWheelTimer::scheduleTimeout(reactor::Reactor& reactor, Fn f, time_t timeout)
{
    struct TimeoutHandlerWrapper : public reactor::TimeoutHandler
    {
        TimeoutHandlerWrapper(Reactor& reactor, Fn f) 
            : reactor::TimeoutHandler(reactor), fn_(std::move(f)){}
        int handle_timeout(int) noexcept override
        {
            try{ fn_(); }
            catch(const std::exception& e) { throw; }
            catch(...){ throw; }//for noexcept
            delete this;
        }
        Fn fn_;
    };

    TimeoutHandlerWrapper *handler = new TimeoutHandlerWrapper(f);
    scheduleTimeout(*handler, timeout);
}

}//util
#endif // _UTIL_HHWHEELTIMER_H_