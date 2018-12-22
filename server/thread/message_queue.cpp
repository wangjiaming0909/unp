#include "message_queue.h"

using namespace thread;


message_queue::message_queue(
    size_t hwm, size_t lwm) 
    : high_water_mark_(hwm)
    , low_water_mark_(lwm)
    , deque_ptr_(new std::deque<util::string>{})
    , mutex_()
    , not_empty_cv_()
    , not_full_cv_(){
}
int message_queue::open(){

}
int message_queue::close(){
    deque_ptr_->clear();
}
message_queue::~message_queue(){ }
int message_queue::enqueue_head(const util::string& message, const microseconds& timeout){
    guard_type _{mutex_};
    if(wait_not_full_condition(timeout) != 0)//timeout
        return -1;
    deque_ptr_->push_front(message);
    return 0;
}
int message_queue::enqueue_tail(const util::string& message, const microseconds& timeout){
    guard_type _{mutex_};
    if(wait_not_full_condition(timeout) != 0)
        return -1;
    deque_ptr_->push_back(message);
    return 0;
}
int message_queue::dequeue_head(util::string*& first_message, const microseconds& timeout){
    guard_type _{mutex_};
    int result = 0;
    if(wait_not_empty_condition(timeout) != 0)//timeout
        return -1;
    *first_message = deque_ptr_->front();
    deque_ptr_->pop_front();
    return 0;
}
int message_queue::dequeue_tail(util::string*& dequeued, const microseconds& timeout){
    guard_type _{mutex_};
    if(wait_not_empty_condition(timeout) != 0)//timeout
        return -1;
    *dequeued = deque_ptr_->back();
    deque_ptr_->back();
    return 0;
}
int message_queue::wait_not_empty_condition(const microseconds& timeout){
    lock_type u_lock{mutex_, std::adopt_lock_t{}};
    int result = 0;
    while(this->deque_ptr_->size() == 0){
        if(not_empty_cv_.wait_for(u_lock, timeout) == std::cv_status::timeout){
            result = -1;
            break;
        }
    }
    return result;
}
int message_queue::wait_not_full_condition(const microseconds& timeout){
    lock_type u_lock{mutex_, std::adopt_lock_t{}};
    int result = 0;
    while(this->deque_ptr_->size() >= high_water_mark_){
        if(not_full_cv_.wait_for(u_lock, timeout) == std::cv_status::timeout){
            result = -1;
            break;
        }
    }
    return result;
}