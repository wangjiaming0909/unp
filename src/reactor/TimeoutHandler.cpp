#include "reactor/TimeoutHandler.h"
#include "reactor/reactor.h"
#include "reactor/HHWheelTimer.h"
#include "util/easylogging++.h"

namespace reactor
{

TimeoutHandler::TimeoutHandler(Reactor& reactor, bool needDestroy) 
  : EventHandler(reactor)
  , needDestroy_(needDestroy)
{ }

TimeoutHandler::~TimeoutHandler()
{
  LOG(INFO) << "destructoring TimeoutHandler...";
}


int TimeoutHandler::handle_timeout(int) noexcept
{
  try {
    if(timeoutCallback) timeoutCallback(this);
  } catch(const std::exception& e) {
    LOG(INFO) << "TimeoutHandler::handle_timeout got error: " << e.what();
  } catch(...) {
    LOG(INFO) << "TimeoutHandler::handle_timeout got unknow error: ";
  }

  isRegistered_ = false;
  wheel_->timeoutExpired(this);
  if(needDestroy()) {
    // LOG(INFO) << "preparing to destructoring TimeoutHandler...";
    destroy();
  }
  return 0;
}


}
