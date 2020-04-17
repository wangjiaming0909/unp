#include "syncclient/timer_handler.h"
#include "reactor/HHWheelTimer.h"


namespace filesync
{
int TimerHandler::handle_timeout(int) noexcept
{
  using namespace reactor;
  auto ret = TimeoutHandler::handle_timeout(-1);
  if (ret < 0 ) return ret;
  wheel_->scheduleTimeout(*this, interval_);
  return 0;
}
}
