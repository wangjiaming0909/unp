#ifndef _UNP_REACTOR_TIMEOUT_HANDLER_H_
#define _UNP_REACTOR_TIMEOUT_HANDLER_H_

#include "reactor/EventHandler.h"
#include "boost/intrusive/list_hook.hpp"
#include "boost/intrusive/list_hook.hpp"
#include <chrono>

namespace reactor
{
class HHWheelTimer;
class TimeoutHandler;

using boost_list_base_hook_t = boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;
class TimeoutHandler : public EventHandler, public boost_list_base_hook_t, public boost::noncopyable
{
    friend class HHWheelTimer;
public:
    using TimePoint_T = std::chrono::steady_clock::time_point;
    TimeoutHandler() = default;
    TimeoutHandler(Reactor& reactor);
    virtual ~TimeoutHandler();

    //timeout handler specific functions
    void setSheduled(HHWheelTimer* wheel, const TimePoint_T& timeout);
    TimePoint_T expirationTimePoint() const { return expiration_; }
    bool isScheduled() const {return wheel_ != nullptr;}

protected:
#ifdef TESTING
public:
#endif
    int bucket_{-1};
    int slotInBucket_{-1};
    HHWheelTimer *wheel_{nullptr};
    TimePoint_T expiration_{};

public: //for std::greater
    bool operator>(const TimeoutHandler &other) const { return expiration_ > other.expiration_; }
};

}
#endif //_UNP_REACTOR_TIMEOUT_HANDLER_H_