#include "HHWheelTimer.h"
#include "reactor/reactor.h"

namespace reactor{


// void Buckets::initializeUnsetBuckets(size_t bucketSize)
// {
//     unsetBuckets_.resize(bucketSize);
//     for(auto &v : unsetBuckets_)
//     {
//         v.resize(1 << (sizeof(SlotSize_t) * 8));
//     }
// }

// bool Buckets::setRegistered(size_t bucket, SlotSize_t slot)
// {
//     if(bucket > buckets_.size()) return false;
//     buckets_[bucket].push(slot);
// //    LOG(INFO) << "Set registered, bucket: " << bucket << " slot: " << static_cast<int>(slot);
//     countOfSlotSet_++;
//     return true;
// }

// bool Buckets::unsetRegistered(size_t bucket, SlotSize_t slot)
// {
//     if(bucket > buckets_.size()) return false;

//     auto &value = unsetBuckets_[bucket][slot];
//     if(value != 0) return false;
//     countOfSlotSet_--;
// //    LOG(INFO) << "Unset Registered, bucket: " << bucket << " slot: " << static_cast<int>(slot);

//     //if the first timer in the minheap of the specified bucket is in the same slot as {slot},
//     //pop it and no need to set the value in the unsetBuckets_
//     if(!buckets_[bucket].empty() && buckets_[bucket].top() == slot)
//     {
//         buckets_[bucket].pop();
//         return true;
//     }
//     value = 1; return true;
// }

// Slot Buckets::findFirstSlot()
// {
//     if(countOfSlotSet_ == 0) return Slot::NotFoundSlot;
//     for (size_t i = 0; i < buckets_.size(); i++)
//     {
//         while(buckets_[i].size() > 0)
//         {
//             if(unsetBuckets_[i][buckets_[i].top()] == 0)
//             {
//                 return {i, buckets_[i].top()};
//             }
//             buckets_[i].pop();//update the state as it has been unseted
//         }
//     }
//     return Slot::NotFoundSlot;
// }

// Slot Buckets::findFirstSlotInFirstBucket()
// {
//     if(buckets_[0].size() == 0) return Slot::NotFoundSlot;
//     while(buckets_[0].size() > 0)
//     {
//         auto min = buckets_[0].top();
//         if(unsetBuckets_[0][min] == 0)
//         {
//             return {0, min};
//         }
//     }
//     return Slot::NotFoundSlot;
// }

// Slot Slot::NotFoundSlot = {SIZE_MAX, UINT8_MAX};


// HHWheelTimer::HHWheelTimer(Reactor& reactor, time_t interval, time_t defaultTimeout)
//     : interval_(interval)
//     , defaultTimeout_(defaultTimeout)
//     , timerCount_(0)
//     , startTime_(getCurTime())
//     , handlers_()
//     , reactor_(&reactor)
//     , registeredBucketsSlots_{WHEEL_BUCKETS}
// {
// }

// HHWheelTimer::HHWheelTimer( time_t interval , time_t defaultTimeout )
//     : interval_(interval)
//     , defaultTimeout_(defaultTimeout)
//     , timerCount_(0)
//     , startTime_(getCurTime())
//     , handlers_()
//     , reactor_(nullptr)
//     , registeredBucketsSlots_{WHEEL_BUCKETS}
// {
// }

// HHWheelTimer::~HHWheelTimer()
// {
// }

// void HHWheelTimer::scheduleTimeout(TimeoutHandler &handler, time_t timeout)
// {
//     auto curT = curTime();
//     auto expireT = curT + timeout;
//     auto baseTick = tickOfCurTime(curT);
//     // should use curTime + timeout, then use the result to calculate expireTick
//     auto thisTimerExpireTick = getTickFromDuration(timeout) + baseTick;
//     scheduleTimeoutImpl_(handler, baseTick, thisTimerExpireTick);
//     handler.setSheduled(this, expireT);

//     if(!isScheduled()) return;//add timeouts into the HHWheelTimer and schedule it later(maybe)

//     // 不一定是当前的这个timeout 会被 放到reactor中
//     scheduleNextTimeoutInReactor_(baseTick, thisTimerExpireTick);
// }

// /**
//  * A little different from HHWheelTimer in folly
//  * When registering timeouts into the reactor,
//  * for folly, HHWheelTimer inherts from AsyncTimeout, so timeoutExpired method is the default callback of timeout event
//  * but, for here, we have TimeoutHandler for handling events, so at default, the reactor (or EventBase) will invoke the handler's callback in the handler, not here
//  * Here is to do some clean up thing for the HHWheelTimer
//  *
//  *
//  * So what should we do when one timeout is expired
//  * 1, remove the timeouts from the wheel
//  * 2, cascade timeouts which are not in the first bucket
//  */
// void HHWheelTimer::timeoutExpired(TimeoutHandler* handler) noexcept
// {
//     //expireTick_ 应与当前超时的timer是同一个时间,
//     // 当此超时处理结束后, expireTick应进行调整, 
//     //如果存在下一个timer,那么使用下一个timer的超时时间,
//     //如果不存在下一个timer,那么结束

//     LOG(INFO) << "Timeout expired, bucket: " << handler->bucket_ << " slot: " << handler->slotInBucket_;
//     timerCount_--;
//     auto slot = registeredBucketsSlots_.findFirstSlot();
//     {
// //        LOG(INFO) << "First slot bucket: " << slot.bucketIndex << " slot: " << static_cast<int>(slot.slotIndex);
// //        LOG(INFO) << "ExpiredTick: " << expireTick_;
//         assert(slot.slotIndex == handler->slotInBucket_);
//     }
//     registeredBucketsSlots_.unsetRegistered(slot.bucketIndex, slot.slotIndex);
//     handler->unlink();
//     auto curT = curTime();
//     auto baseTick = tickOfCurTime(curT);
//     if(timerCount_ == 0) return;

//     auto firstSlot = registeredBucketsSlots_.findFirstSlot();
//     if(firstSlot == Slot::NotFoundSlot) return;

//     auto handlersList = &handlers_[firstSlot.bucketIndex][firstSlot.slotIndex];
//     auto thisTimerExpireTick = (handlersList->front().expiration_ - curT) / interval_;
//     expireTick_ = thisTimerExpireTick < 0 ? 0 : thisTimerExpireTick;
//     LOG(INFO) << "Setting expireTick2_: " << expireTick_;

//     intrusive_list_t tmp;
//     tmp.swap(*handlersList);
//     while(!tmp.empty())
//     {
//         LOG(INFO) << "1";
//         auto front = &tmp.front();
//         tmp.pop_front();
//         registeredBucketsSlots_.unsetRegistered(static_cast<size_t>(front->bucket_),
//                                                 static_cast<Slot::SlotSize_t>(front->slotInBucket_));
//         timerCount_--;
//         scheduleTimeoutImpl_(*front, baseTick, thisTimerExpireTick);
//     }
//     scheduleNextTimeoutInReactor_(baseTick, thisTimerExpireTick);
// }

// void HHWheelTimer::cancelAll()
// {
//     if(timerCount_ == 0) return;
//     for (auto& bucket : handlers_) {
//         for(auto& slot : bucket)
//         {
//             if(slot.empty()) continue;
//             intrusive_list_t tmp;
//             tmp.swap(slot);
//             cancelTimeoutsFromList(tmp);
//             timerCount_--;
//             expireTick_ = INT64_MAX;
//         }
//     }

// }

// size_t HHWheelTimer::cancelTimeoutsFromList(intrusive_list_t& handlers)
// {
//     size_t size = 0;
//     while(!handlers.empty())
//     {
//         auto front = &handlers.front();
//         handlers.pop_front();
//         size++;
//         registeredBucketsSlots_.unsetRegistered(static_cast<size_t>(front->bucket_),
//                                                 static_cast<Slot::SlotSize_t>(front->slotInBucket_));
//         if(front->isRegistered())
//             front->reactor_->unregister_handler(front, EventHandler::TIMEOUT_EVENT);
//     }
//     return size;
// }

// /**
//  * schedule timeout时, 当前timeout所放的位置与什么有关?
//  * 1, 与当前时间所在的tick有关
//  * 2, 与所指定的timeout有关
//  * */
// void HHWheelTimer::scheduleTimeoutImpl_(TimeoutHandler& handler, int64_t baseTick, int64_t thisTimerExpireTick)
// {
//     auto ticksToGo = thisTimerExpireTick - baseTick;
//     intrusive_list_t *pos;
//     /**
//      * scheduleTimeout时可能有几种情况:
//      * 1, 可能schedule的timeout正好在当前时间所在的tick上, 也就是所传递的timeout与curTime在同一个interval中
//      */

//     if(ticksToGo < 0)//it means that the timeout has already timed out, so do nothing
//     {
//         pos = &handlers_[0][0];
//         registeredBucketsSlots_.setRegistered(0, 0);
//         handler.bucket_ = 0;
//         handler.slotInBucket_ = 0;
//     }
//     else if(ticksToGo < WHEEL_SIZE)
//     {
//         pos = &handlers_[0][ticksToGo];
//         registeredBucketsSlots_.setRegistered(0, static_cast<uint8_t>(ticksToGo));
//         handler.bucket_ = 0;
//         handler.slotInBucket_ = static_cast<int>(ticksToGo);
//     }
//     else if(ticksToGo < (1 << 2 * WHEEL_BITS))
//     {
//         pos = &handlers_[1][ticksToGo >> (WHEEL_BITS)];
//         registeredBucketsSlots_.setRegistered(1, static_cast<uint8_t>(ticksToGo >> (WHEEL_BITS)));
//         handler.bucket_ = 1;
//         handler.slotInBucket_ = static_cast<int>(ticksToGo >> WHEEL_BITS);
//     }
//     else if(ticksToGo < (1 << (3 * WHEEL_BITS)))
//     {
//         pos = &handlers_[2][ticksToGo >> (2 * WHEEL_BITS)];
//         registeredBucketsSlots_.setRegistered(2, static_cast<uint8_t>(ticksToGo >> (2 * WHEEL_BITS)));
//         handler.bucket_ = 2;
//         handler.slotInBucket_ = static_cast<int>(ticksToGo >> (2 * WHEEL_BITS));
//     }
//     else //currently, the buckets can't take this timeout, cause the time is very long time later
//     {
//         if(ticksToGo > LARGEST_SLOT)
//         {
//             ticksToGo = LARGEST_SLOT;
//         }
//         pos = &handlers_[3][ticksToGo >> (3 * WHEEL_BITS)];
//         registeredBucketsSlots_.setRegistered(3, static_cast<uint8_t>(ticksToGo >> (3 * WHEEL_BITS)));
//         handler.bucket_ = 3;
//         handler.slotInBucket_ = static_cast<int>(ticksToGo >> (3 * WHEEL_BITS));
//     }
//     pos->push_back(handler);
//     timerCount_++;
// }

// void HHWheelTimer::scheduleNextTimeoutInReactor_(int64_t baseTick, int64_t thisTimerExpireTick)
// {
//     if(!isScheduled())
//     {
//         LOG(WARNING) << "No reactor is attached to the HHWheelTimer";
//         return;
//     }

//     /**
//      * Everytime schedule a timeout in the wheel, sometimes we donot need to register it into the reactor
//      * There are some situations:
//      * 1, There is no timeout event registered into the reactor(EventBase)
//      * 2, Already registered timeout events into the reactor, The timeout we are scheduling is sooner than the expire time of last timeout
//      *  if we do not register this sooner timer into the reactor, we'll miss it
//      * 3, Already registered timeout events into the reactor, and the timer we are scheduling is later than last timeout,
//      * we do not register it into the reactor, wait until the first timeout and invoke timeoutExpired, than schedule the soonest timeout
//      */
//     // we assert that the timeouts event in the reactor are registered by this wheel if reactor has timeout events

//     //if no next timeout in the wheel or no registedSlot in buckets, then return
//     if(timerCount_ == 0 || registeredBucketsSlots_.count() == 0) return;

//     /*
//      * if there is no timeout in the first bucket, but has other timeouts in other bucket
//      * so we schedule a default TimeoutHandler to wake self up,
//      * then, we'll cascade other the timeouts which are not in the first bucket
//      */

//     auto firstSlotInFirstBucket = registeredBucketsSlots_.findFirstSlotInFirstBucket();
//     if(firstSlotInFirstBucket == Slot::NotFoundSlot)
//     {
//         int64_t ticksToGo = (1 << WHEEL_BITS) - baseTick - 1;
//         time_t timeout = ticksToGo * interval_;
//         scheduleTimeoutFn([](){(void)0;}, timeout);
//         return;
//     }

//     // Find the timeouts that will expire first in the first bucket
//     //!this first timeout could have been registered into the reactor, so how to exclude them
//     auto firstHandlerListInWheel = &handlers_[firstSlotInFirstBucket.bucketIndex][firstSlotInFirstBucket.slotIndex];
// //    LOG(INFO) << "Get first slot, bucket: "
// //              << firstSlotInFirstBucket.bucketIndex
// //              << " slot: " << static_cast<int>(firstSlotInFirstBucket.slotIndex);

//     //1,2
//     LOG(INFO) << "expireTick: " << expireTick_ << " thisTimerExpireTick: " << thisTimerExpireTick;
//     LOG(INFO) << "has timeout Event in reactor: " << reactor_->hasEvent(EventHandler::TIMEOUT_EVENT);
//     if(!reactor_->hasEvent(EventHandler::TIMEOUT_EVENT) || expireTick_ >= thisTimerExpireTick)
//     {
//         LOG(INFO) << "size: " << firstHandlerListInWheel->size();
//         for(auto &timeoutHandler : *firstHandlerListInWheel)
//         {
//             if(!timeoutHandler.isRegistered())
//             {
//                 LOG(INFO) << "Registering timeout Handler, bucket: " << firstSlotInFirstBucket.bucketIndex
//                           << " slot: " << static_cast<int>(firstSlotInFirstBucket.slotIndex);
//                 if(expireTick_ > firstSlotInFirstBucket.slotIndex)
//                 {
//                     auto slotTmp = static_cast<int64_t>(firstSlotInFirstBucket.slotIndex);
//                     expireTick_ = slotTmp < 0 ? 0 : slotTmp;
//                     LOG(INFO) << "Setting expiredTick_ to: " << expireTick_;
//                 }
//                 reactor_->register_handler(&timeoutHandler, EventHandler::TIMEOUT_EVENT);
//             }
//         }
//     }
// }

// inline int64_t  HHWheelTimer::tickOfCurTime(const time_point_t& curTime) const
// {
//     return (curTime - startTime_) / interval_;
// }

// inline int64_t HHWheelTimer::tickOfCurTime() const
// {
//     return tickOfCurTime(curTime());
// }

// inline HHWheelTimer::time_point_t HHWheelTimer::curTime() const
// {
//     return getCurTime();
// }

// inline HHWheelTimer::time_point_t getCurTime() { return std::chrono::steady_clock::now(); }

} // reactor