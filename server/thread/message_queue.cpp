#include "message_queue.h"

using namespace thread;


message_queue::message_queue(
    size_t hwm, size_t lwm) 
    : high_water_mark_(hwm)
    , low_water_mark_(lwm){

}
int message_queue::open(){

}
int message_queue::close(){

}
message_queue::~message_queue(){

}
int message_queue::enqueue_head(const util::string& message){

}
int message_queue::enqueue_tail(const util::string& message){

}
int message_queue::dequeue_head(util::string*& first_message){

}
int message_queue::dequeue_tail(util::string*& dequeued){

}