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
    TimeoutHandler() = default;
    TimeoutHandler(Reactor& reactor);
    virtual ~TimeoutHandler();

    void setSheduled(HHWheelTimer* wheel, std::chrono::microseconds timeout);
    std::chrono::steady_clock::time_point expirationTimePoint() const { return expiration_; }

protected:
    int posInBucket{-1};
    HHWheelTimer *wheel_{nullptr};
    std::chrono::steady_clock::time_point expiration_{}; 

public: //for std::greater
    bool operator>(const TimeoutHandler &other) const { return expiration_ > other.expiration_; }
};

}
#endif //_UNP_REACTOR_TIMEOUT_HANDLER_H_