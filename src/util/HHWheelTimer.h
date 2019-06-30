#ifndef _UTIL_HHWHEELTIMER_H_
#define _UTIL_HHWHEELTIMER_H_
#include <stdint.h>
#include <chrono>
#include "reactor/event_handler.h"

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
    void scheduleTimeout(Fn f, time_t timeout);
    void scheduleTimeout(reactor::event_handler &handler, time_t timeout);

protected:
    ~HHWheelTimer();
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
void HHWheelTimer::scheduleTimeout(Fn f, time_t timeout)
{
    struct TimeoutHandler : public reactor::event_handler
    {
        TimeoutHandler(Fn f) : fn_(std::move(f)){}
        int handle_timeout(int) noexcept override
        {
            try{ fn_(); }
            catch(const std::exception& e) { throw; }
            catch(...){ throw; }
            delete this;
        }
        Fn fn_;
    };

    TimeoutHandler *handler = new TimeoutHandler(f);
    scheduleTimeout(*handler, timeout);
}

}//util
#endif // _UTIL_HHWHEELTIMER_H_