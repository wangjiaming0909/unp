#ifndef _THREAD_MESSAGE_QUEUE_H_
#define _THREAD_MESSAGE_QUEUE_H_

#include <deque>
#include <mutex>
#include <memory>
#include "server/util/XString.h"
#include <chrono>
#include "server/thread/lock_guard.h"
#include <condition_variable>

namespace thread{
class message_queue{
public:
    using microseconds = std::chrono::microseconds;
    using mutex_type = std::mutex;
    using guard_type = lock_guard<mutex_type>;
    using cv_type = std::condition_variable;
    using lock_type = std::unique_lock<mutex_type>;
    message_queue(size_t hwm, size_t lwm);
    int open();
    int close();
    ~message_queue();
    int enqueue_head(const util::string& message, const microseconds& timeout);
    int enqueue_tail(const util::string& message, const microseconds& timeout);
    int dequeue_head(util::string*& first_message, const microseconds& timeout);
    int dequeue_tail(util::string*& dequeued, const microseconds& timeout);
    bool is_full() const {return deque_ptr_->size() == high_water_mark_;}
    bool is_empty() const {return deque_ptr_->size() == 0;}
    size_t current_message_count() const {return deque_ptr_->size();}

protected:
    int wait_not_empty_condition(const microseconds& timeout);
    int wait_not_full_condition(const microseconds& timeout);

private:
    size_t                                          high_water_mark_;
    size_t                                          low_water_mark_;
    mutex_type	                                    mutex_;
    std::shared_ptr<std::deque<util::string>>       deque_ptr_;
    cv_type	                                        not_empty_cv_;
    cv_type	                                        not_full_cv_;

};
}//namespace thread

#endif // _THREAD_MESSAGE_QUEUE_H_