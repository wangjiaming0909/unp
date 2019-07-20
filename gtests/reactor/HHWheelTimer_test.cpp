#include <gtest/gtest.h>
#include <string>
#include "reactor/HHWheelTimer.h"
#include "reactor/reactor.h"
#include "reactor/select_reactor_impl.h"
#include "util/easylogging++.h"

using namespace std::chrono_literals;

struct FakeTimeoutHandler : public reactor::TimeoutHandler{
    FakeTimeoutHandler(reactor::Reactor& react, const std::string& name)
        : TimeoutHandler(react)
        , name(name){}

    int state = -1;
    std::string name;
};

void timeoutCallback(reactor::TimeoutHandler* handler)
{
    dynamic_cast<FakeTimeoutHandler*>(handler)->state = 0;
    LOG(INFO) << "timer expired...";
}

void timeoutCallback22(reactor::TimeoutHandler* handler)
{
    LOG(INFO) << "----------------------timer expired...";
}

TEST(HHWheelTimer, normal_test)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}, true};
    TimeoutHandler *handler1 = new FakeTimeoutHandler{react, "1"};
    TimeoutHandler *handler2 = new FakeTimeoutHandler{react, "2"};
    handler1->timeoutCallback = timeoutCallback;
    handler2->timeoutCallback = timeoutCallback22;

    HHWheelTimer *timer = new HHWheelTimer{&react};
    ASSERT_TRUE(timer != nullptr);

    timer->scheduleTimeout(*handler1, 2ms);
    timer->scheduleTimeout(*handler2, std::chrono::duration_cast<HHWheelTimer::Duration>(1s));
    ASSERT_EQ(timer->getTimerCount(), 2);

    auto timeout = 2000000us;
    int i = 0;
    while(i < 2)
    {
        react.handle_events(&timeout);
        i++;
    }
    ASSERT_EQ(timer->getTimerCount(), 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler1)->state, 0);
}


void timeoutCallback1(reactor::TimeoutHandler* handler)
{
    auto fake = dynamic_cast<FakeTimeoutHandler*>(handler);
    fake->state = 0;
    ASSERT_STREQ(fake->name.c_str(), "1");
    LOG(INFO) << "callback1...";
}

void timeoutCallback2(reactor::TimeoutHandler* handler)
{
    auto fake = dynamic_cast<FakeTimeoutHandler*>(handler);
    fake->state = 0;
    ASSERT_STREQ(fake->name.c_str(), "2");
    LOG(INFO) << "callback2...";
}

void timeoutCallback3(reactor::TimeoutHandler* handler)
{
    auto fake = dynamic_cast<FakeTimeoutHandler*>(handler);
    fake->state = 0;
    ASSERT_STREQ(fake->name.c_str(), "3");
    LOG(INFO) << "callback3...";
}

void timeoutCallback4(reactor::TimeoutHandler* handler)
{
    auto fake = dynamic_cast<FakeTimeoutHandler*>(handler);
    fake->state = 0;
    LOG(INFO) << "callback4...";
}

TEST(HHWheelTimer, schedule_multiTimeouts)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}, true};
    TimeoutHandler *handler1 = new FakeTimeoutHandler{react, "1"};
    TimeoutHandler *handler2 = new FakeTimeoutHandler{react, "2"};
    TimeoutHandler *handler3 = new FakeTimeoutHandler{react, "3"};
    TimeoutHandler *handler4 = new FakeTimeoutHandler{react, "4"};
    handler1->timeoutCallback = timeoutCallback1;
    handler2->timeoutCallback = timeoutCallback2;
    handler3->timeoutCallback = timeoutCallback3;
    handler4->timeoutCallback = timeoutCallback4;

    HHWheelTimer *timer = new HHWheelTimer{&react};

    timer->scheduleTimeout(*handler1, 1s);
    ASSERT_EQ(timer->getTimerCount(), 1);
    timer->scheduleTimeout(*handler2, 2s);
    ASSERT_EQ(timer->getTimerCount(), 2);
    timer->scheduleTimeout(*handler3, 3005ms);
    ASSERT_EQ(timer->getTimerCount(), 3);
    timer->scheduleTimeout(*handler4, 2222ms);
    ASSERT_EQ(timer->getTimerCount(), 4);

    auto timeout = 1us;
    int i = 0;
    while(i < 6)
    {
        react.handle_events(&timeout);
        i++;
    }
    ASSERT_EQ(timer->getTimerCount(), 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler1)->state, 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler2)->state, 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler3)->state, 0);
}

TEST(HHWheelTimer, scheduleTimeout_usingFunction)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}, true};

    HHWheelTimer *timer = new HHWheelTimer{&react};
    ASSERT_TRUE(timer != nullptr);

    //schedule timeout using function
    int state = -1;
    timer->scheduleTimeoutFn([&](TimeoutHandler*){state = 1;}, 1s);
    ASSERT_EQ(timer->getTimerCount(), 1);

    auto timeout = 200000000000000us;
    react.handle_events(&timeout);
    ASSERT_EQ(timer->getTimerCount(), 0);
    ASSERT_EQ(state, 1);
}

TEST(HHWheelTimer, cancel)
{
    // using namespace reactor;
    // Reactor react{new select_reactor_impl{}, true};

    // HHWheelTimer *timer = new HHWheelTimer{&react};
    // ASSERT_TRUE(timer != nullptr);

    // FakeTimeoutHandler* timeoutHandler = new FakeTimeoutHandler(react, "1");
    // timer->scheduleTimeout(*timeoutHandler, 1s);
    // bool hasTimeroutEvent = react.hasEvent(EventHandler::TIMEOUT_EVENT);
    // ASSERT_EQ(hasTimeroutEvent, true);
    // ASSERT_EQ(timer->getTimerCount(), 1);

    // timer->cancelAll();
    // ASSERT_EQ(timer->getTimerCount(), 0);
    // hasTimeroutEvent = react.hasEvent(EventHandler::TIMEOUT_EVENT);
    // ASSERT_EQ(hasTimeroutEvent, false);
}

TEST(HHWheelTimer, scheduleTimeout_with_reverse_order)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}, true};

    HHWheelTimer *timer = new HHWheelTimer{&react};

    TimeoutHandler *handler1 = new FakeTimeoutHandler{react, "1"};
    TimeoutHandler *handler2 = new FakeTimeoutHandler{react, "2"};
    TimeoutHandler *handler3 = new FakeTimeoutHandler{react, "3"};
    TimeoutHandler *handler4 = new FakeTimeoutHandler{react, "4"};
    handler1->timeoutCallback = timeoutCallback1;
    handler2->timeoutCallback = timeoutCallback2;
    handler3->timeoutCallback = timeoutCallback3;
    handler4->timeoutCallback = timeoutCallback4;

    int value = 0;

    timer->scheduleTimeout(*handler1, 3s);
//    ASSERT_EQ(timer->getTimerCount(), 1);
    timer->scheduleTimeout(*handler2, 1800ms);
    timer->scheduleTimeoutFn([&](TimeoutHandler*){
        value++;
        LOG(INFO) << "*+*+*+*+*+*+*+**+ fn1";
    }, 2000ms);
    timer->scheduleTimeoutFn([&](TimeoutHandler*){
        value++;
        LOG(INFO) << "*+*+*+*+*+*+*+**+ fn2";
    }, 2100ms);
    timer->scheduleTimeoutFn([&](TimeoutHandler*){
        value++;
        LOG(INFO) << "*+*+*+*+*+*+*+**+ fn3";
    }, 2200ms);
//    ASSERT_EQ(timer->getTimerCount(), 2);
    timer->scheduleTimeout(*handler3, 1s);
//    ASSERT_EQ(timer->getTimerCount(), 3);
    timer->scheduleTimeoutFn([&](TimeoutHandler*){
        value++;
        LOG(INFO) << "*+*+*+*+*+*+*+**+ fn4";
    }, 4000ms);
    timer->scheduleTimeout(*handler4, 7020ms);
    timer->scheduleTimeoutFn([&](TimeoutHandler*){
        value++;
        LOG(INFO) << "*+*+*+*+*+*+*+**+ fn5";
    }, 8000ms);

    auto timeout = 1us;
    int i = 0;
    while(i < 15)
    {
        react.handle_events(&timeout);
        i++;
    }
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler1)->state, 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler2)->state, 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler3)->state, 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler4)->state, 0);

    ASSERT_EQ(value, 5);

}

TEST(HHWheelTimer, timeoutExpired){
    using namespace reactor;
    Reactor react{new select_reactor_impl{}, true};

    HHWheelTimer timer{&react};

    TimeoutHandler *handler1 = new FakeTimeoutHandler{react, "1"};
    TimeoutHandler *handler2 = new FakeTimeoutHandler{react, "2"};
    TimeoutHandler *handler3 = new FakeTimeoutHandler{react, "3"};

    timer.scheduleTimeout(*handler1, 3s);
    // timer.scheduleTimeout(*handler2, 1s);
    // timer.scheduleTimeout(*handler3, 2s);

    timer.timeoutExpired(handler1);
}