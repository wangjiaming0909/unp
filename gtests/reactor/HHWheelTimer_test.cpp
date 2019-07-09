#include <gtest/gtest.h>
#include "reactor/HHWheelTimer.h"
#include "reactor/reactor.h"
#include "reactor/select_reactor_impl.h"

using namespace std::chrono_literals;

class FakeTimeoutHandler : public reactor::TimeoutHandler{
public:
    FakeTimeoutHandler(reactor::Reactor& react) : TimeoutHandler(react){}

    virtual int handle_timeout(int ) noexcept override 
    {
        wheel_->timeoutExpired();
        state = 0;
        return -1;
    }

    virtual int handle_close(int) override
    {
    }

    int state = -1;
};

TEST(HHWheelTimer, normal_test)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}, true};
    TimeoutHandler *handler = new FakeTimeoutHandler{react};

    HHWheelTimer *timer = new HHWheelTimer{react};
    ASSERT_TRUE(timer != nullptr);

    timer->scheduleTimeout(*handler, 1s);

    auto timeout = 200000000000000us;
    react.handle_events(&timeout);

    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler)->state, 0);
}
