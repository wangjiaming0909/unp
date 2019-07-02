#include "HHWheelTimer.h"

namespace reactor{

int HHWheelTimer::DEFAULT_TICK_INTERVAL = 10;

HHWheelTimer::HHWheelTimer(Reactor& reactor, time_t interval, time_t defaultTimeout) 
    : interval_(interval)
    , defaultTimeout_(defaultTimeout)
    , currentTick_(1)
    , timerCount_(0)
    , startTime_(getCurTime())
    , handlers_()
    , reactor_(reactor)
{
}

HHWheelTimer::~HHWheelTimer()
{
}

void HHWheelTimer::scheduleTimeout(TimeoutHandler &handler, time_t timeout)
{
    
}

void HHWheelTimer::registerIntoReactor()
{

}

} // util