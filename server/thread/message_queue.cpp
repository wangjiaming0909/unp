#include "message_queue.h"

using namespace thread;


message_queue::message_queue(
    size_t hwm, size_t lwm) 
    : high_water_mark_(hwm)
    , low_water_mark_(lwm)
    , deque_ptr_(new std::deque<util::string>{})
    , lock_(){
}
int message_queue::open(){

}
int message_queue::close(){
    deque_ptr_->clear();
}
message_queue::~message_queue(){

}
int message_queue::enqueue_head(const util::string& message, const microseconds& timeout){
    if(deque_ptr_->size() >= high_water_mark_)
        return -1;
    guard_type _{lock_};
    deque_ptr_->push_front(message);
    return 0;
}
int message_queue::enqueue_tail(const util::string& message, const microseconds& timeout){
    deque_ptr_->push_back(message);
}
int message_queue::dequeue_head(util::string*& first_message, const microseconds& timeout){
    *first_message = deque_ptr_->front();
    deque_ptr_->pop_front();
}
int message_queue::dequeue_tail(util::string*& dequeued, const microseconds& timeout){
    *dequeued = deque_ptr_->back();
    deque_ptr_->back();
}