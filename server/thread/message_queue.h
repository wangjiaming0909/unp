#ifndef _THREAD_MESSAGE_QUEUE_H_
#define _THREAD_MESSAGE_QUEUE_H_

#include <deque>
#include <mutex>
#include "../util/XString.h"

namespace thread{
class message_queue{
public:
    message_queue(size_t hwm, size_t lwm);
    int open();
    int close();
    ~message_queue();
    int enqueue_head(const util::string& message);
    int enqueue_tail(const util::string& message);
    int dequeue_head(util::string*& first_message);
    int dequeue_tail(util::string*& dequeued);
    bool is_full() const {return deque_.size() == high_water_mark_;}
    bool is_empty() const {return deque_.size() == 0;}
    size_t current_message_count() const {return deque_.size();}
private:
    size_t                      high_water_mark_;
    size_t                      low_water_mark_;
    std::mutex	                lock_;
    std::deque<util::string>    deque_;
};
}//namespace thread

#endif // _THREAD_MESSAGE_QUEUE_H_