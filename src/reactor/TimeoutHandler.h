#ifndef _UNP_REACTOR_TIMEOUT_HANDLER_H_
#define _UNP_REACTOR_TIMEOUT_HANDLER_H_

#include "reactor/EventHandler.h"
#include "boost/intrusive/list_hook.hpp"
#include <chrono>

namespace reactor
{
class HHWheelTimer;
using boost_list_base_hook_t = boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;

class TimeoutHandler : public EventHandler, public boost_list_base_hook_t, public boost::noncopyable
{
public:
    friend class HHWheelTimer;
    using Duration = std::chrono::milliseconds;
    using TimePoint_T = std::chrono::steady_clock::time_point;
    using WheelTimer_t = HHWheelTimer;
    TimeoutHandler(bool needDestroy = false) : needDestroy_(needDestroy){}
    TimeoutHandler(Reactor& reactor, bool needDestroy = false);
    virtual ~TimeoutHandler();

    virtual int handle_timeout(int) noexcept override;

    //timeout handler specific functions
    void setSheduled(WheelTimer_t &wheel, const TimePoint_T& timeout)
    {
        wheel_ = &wheel;
        expiration_ = timeout;
    }
    TimePoint_T expirationTimePoint() const { return expiration_; }
    bool needDestroy() const {return needDestroy_;}
    void destroy(){delete this;}
public:
    std::function<void(TimeoutHandler*)> timeoutCallback{};

protected:
#ifdef TESTING
public:
#endif
    int slotInBucket_{-1};
    WheelTimer_t *wheel_{nullptr};
    TimePoint_T expiration_{};
    bool needDestroy_{false};

//override hiding functions
private:
    int handle_input(int) override { return 0; }
    int handle_output(int) override { return 0; }
    int handle_close(int) override { return 0; }
    int close_read(int) override { return 0; }
    int close_write(int) override { return 0; }
    int handle_signal(int) override{ return 0; }
    int get_handle() const override{ return 0; }
    void set_handle(int) override {}
};

//for std::greater<TimeoutHandler>
template <typename handler>
struct TimeoutHandlerComparer
{
    bool operator()(const handler& handler1, const handler& handler2)
    {
        return handler1.expirationTimePoint() > handler2.expirationTimePoint();
    }
};

template <typename handler>
struct TimeoutHandlerComparer<handler*>
{
    bool operator()(handler*& handler1, handler*& handler2)
    {
        return handler1->expirationTimePoint() > handler2->expirationTimePoint();
    }
};

}
#endif //_UNP_REACTOR_TIMEOUT_HANDLER_H_
