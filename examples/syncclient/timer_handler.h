#pragma once

#include "reactor/TimeoutHandler.h"
#include "reactor/reactor.h"

namespace filesync
{

using namespace reactor;
class TimerHandler : public reactor::TimeoutHandler
{
public:
  TimerHandler(Reactor& react, TimeoutHandler::Duration interval) 
    : TimeoutHandler(react)
    , interval_(interval)
  {}
  virtual int handle_timeout(int) noexcept override;

private:
  TimeoutHandler::Duration interval_;
};

}
