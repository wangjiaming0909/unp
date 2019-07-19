#ifndef _UNP_REACTOR_TIMEOUT_HANDLER_H_
#define _UNP_REACTOR_TIMEOUT_HANDLER_H_

#include "reactor/EventHandler.h"
#include "boost/intrusive/list_hook.hpp"
#include "boost/intrusive/list_hook.hpp"
#include "reactor/HHWheelTimer.h"
#include <chrono>

namespace reactor
{
using boost_list_base_hook_t = boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;
class TimeoutHandler : public EventHandler, public boost_list_base_hook_t, public boost::noncopyable
{
    friend class HHWheelTimer;
public:
    using TimePoint_T = std::chrono::steady_clock::time_point;
    using WheelTimer_t = HHWheelTimer<Duration>;
    TimeoutHandler() = default;
    TimeoutHandler(Reactor& reactor);
    virtual ~TimeoutHandler();

    //timeout handler specific functions
    void setSheduled(WheelTimer_t* wheel, const TimePoint_T& timeout);
    TimePoint_T expirationTimePoint() const { return expiration_; }
    bool isScheduled() const {return wheel_ != nullptr;}

protected:
#ifdef TESTING
public:
#endif
    int bucket_{-1};
    int slotInBucket_{-1};
    WheelTimer_t *wheel_{nullptr};
    TimePoint_T expiration_{};
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

TimeoutHandler::TimeoutHandler(Reactor& reactor) : EventHandler(reactor)
{
}

TimeoutHandler::~TimeoutHandler()
{
}


void TimeoutHandler::setSheduled(HHWheelTimer* wheel, const TimePoint_T& timeout)
{
    assert(wheel != nullptr);
    wheel_ = wheel;
    expiration_ = timeout;
}

}
#endif //_UNP_REACTOR_TIMEOUT_HANDLER_H_