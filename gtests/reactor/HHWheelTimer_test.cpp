#include <gtest/gtest.h>
#include <string>
#include "reactor/HHWheelTimer.h"
#include "reactor/reactor.h"
#include "reactor/select_reactor_impl.h"

using namespace std::chrono_literals;

class FakeTimeoutHandler : public reactor::TimeoutHandler{
public:
    FakeTimeoutHandler(reactor::Reactor& react, const std::string& name)
        : TimeoutHandler(react)
        , name(name){}

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
    std::string name;
};

TEST(HHWheelTimer, normal_test)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}, true};
    TimeoutHandler *handler = new FakeTimeoutHandler{react, "1"};

    HHWheelTimer *timer = new HHWheelTimer{react};
    ASSERT_TRUE(timer != nullptr);

    timer->scheduleTimeout(*handler, 2s);
    ASSERT_EQ(timer->getTimerCount(), 1);
    ASSERT_EQ(timer->isScheduled(), true);


    auto timeout = 200000000000000us;
    react.handle_events(&timeout);
    ASSERT_EQ(timer->getTimerCount(), 0);

    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler)->state, 0);
}

TEST(HHWheelTimer, schedule_multiTimeouts)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}, true};
    TimeoutHandler *handler1 = new FakeTimeoutHandler{react, "1"};
    TimeoutHandler *handler2 = new FakeTimeoutHandler{react, "2"};
    TimeoutHandler *handler3 = new FakeTimeoutHandler{react, "3"};

    HHWheelTimer *timer = new HHWheelTimer{react};

    timer->scheduleTimeout(*handler1, 1s);
    ASSERT_EQ(timer->getTimerCount(), 1);
    ASSERT_EQ(timer->isScheduled(), true);
    timer->scheduleTimeout(*handler2, 2s);
    ASSERT_EQ(timer->getTimerCount(), 2);
    ASSERT_EQ(timer->isScheduled(), true);
    timer->scheduleTimeout(*handler3, 3s);
    ASSERT_EQ(timer->getTimerCount(), 3);
    ASSERT_EQ(timer->isScheduled(), true);

    auto timeout = 200000000000000us;
    react.handle_events(&timeout);
    ASSERT_EQ(timer->getTimerCount(), 2);
    react.handle_events(&timeout);
    ASSERT_EQ(timer->getTimerCount(), 1);
    react.handle_events(&timeout);
    ASSERT_EQ(timer->getTimerCount(), 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler1)->state, 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler2)->state, 0);
    ASSERT_EQ(dynamic_cast<FakeTimeoutHandler*>(handler3)->state, 0);
}

TEST(HHWheelTimer, scheduleTimeout_usingFunction)
{
    using namespace reactor;
    Reactor react{new select_reactor_impl{}, true};

    HHWheelTimer *timer = new HHWheelTimer{react};
    ASSERT_TRUE(timer != nullptr);

    //schedule timeout using function
    int state = -1;
    timer->scheduleTimeoutFn([&](){state = 1;}, 1s);
    ASSERT_EQ(timer->getTimerCount(), 1);

    auto timeout = 200000000000000us;
    react.handle_events(&timeout);
    ASSERT_EQ(timer->getTimerCount(), 0);
    ASSERT_EQ(state, 1);
}

TEST(BucketsAndSlots, constuctor)
{
    using namespace reactor;
    size_t bucketSize = 4;
    Buckets buckets{bucketSize};
    ASSERT_EQ(buckets.buckets_.size(), bucketSize);
    ASSERT_EQ(buckets.unsetBuckets_.size(), bucketSize);

    for(auto &minHeap : buckets.buckets_)
    {
        ASSERT_EQ(minHeap.size(), 0);
    }

    //unsetBuckets should be initialized properly
    for(auto& v : buckets.unsetBuckets_)
    {
        ASSERT_EQ(v.size(), (1 << (sizeof(Buckets::SlotSize_t) * 8)));
    }
}

TEST(BucketsAndSlots, setRegister)
{
    using namespace reactor;
    size_t bucketSize = 4;
    Buckets b{bucketSize};

    b.setRegistered(0, 11);
    ASSERT_EQ(b.buckets_[0].top(), 11);
    ASSERT_EQ(b.buckets_[0].size(), 1);
    b.setRegistered(0, 10);
    ASSERT_EQ(b.buckets_[0].top(), 10);
    ASSERT_EQ(b.buckets_[0].size(), 2);

    for(auto& v : b.unsetBuckets_)
    {
        ASSERT_EQ(v.size(), (1 << (sizeof(Buckets::SlotSize_t) *8)));
    }
}

TEST(BucketsAndSlots, unsetRegister)
{
    using namespace reactor;
    size_t bucketSize = 4;
    Buckets b{bucketSize};

    b.setRegistered(0, 11);
    b.setRegistered(0, 10);

    ASSERT_EQ(b.buckets_[0].top(), 10);
    b.unsetRegistered(0, 10);
    auto slot = b.findFirstSlot();
    ASSERT_EQ(slot.bucketIndex, 0);
    ASSERT_EQ(slot.slotIndex, 11);
    b.unsetRegistered(0, 11);
    slot = b.findFirstSlot();
    ASSERT_EQ(slot, Slot::NotFoundSlot);
}
