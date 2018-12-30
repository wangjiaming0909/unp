#include "thread_pool.h"

using namespace thread;

thread_pool::thread_pool(size_t number_of_threads)
    : _n_of_threads(number_of_threads),
    _running(false),
    _mutex(),
    _unique_lock(_mutex, std::defer_lock),
    _has_task_cv(),
    _deque_full_cv(),
    _tasks(),
    _threads(0){
}

thread_pool::~thread_pool(){
    if(_running)
        stop(micro_seconds(0));
}

//will we have multi threads to add_tasks?
void thread_pool::add_task(const task& t){
    // log_with_thread_id("trying to get the mutex");
    _unique_lock.lock();
    // log_with_thread_id("get the mutex");
    //put the check inside the lock??
    while(deque_full()){
        _deque_full_cv.wait(_unique_lock);
    }
    //if has space for the new task
    _tasks.push_back(t);
    //if the deque is empty, we need to notify one thread when we add one task
    _has_task_cv.notify_one();
    _unique_lock.unlock();
    // log_with_thread_id("release the mutex");
}

void thread_pool::start(){
    _running = true;
    initialize();
}

//? if timeout == nullptr -> wait infinitly, and all the threads will not close
//? if timeout != nullptr, but the timeout is 0 -> do not wait, close all the threads
//? if timeout != nullptr, and timeout is not 0 -> wait for the timeout, and stop them all
void thread_pool::wait(const micro_seconds* timeout) {
    if(timeout == nullptr) {
        for(auto &t : _threads) {
            if(t->joinable()) t->join();
        }
    } else {
        stop(*timeout);
    }
}

void thread_pool::stop(const micro_seconds& timeout){
    if(timeout.count() != 0){
        std::this_thread::sleep_for(timeout);
    }
    _running = false;
    _has_task_cv.notify_all();
    _deque_full_cv.notify_all();
    for(auto& t : _threads){
        t->detach();
    }
}

//allocate threads
void thread_pool::initialize(){
    for(size_t i = 0; i < _n_of_threads; i++){
        //construct the new thread, let it go the default routine
        std::shared_ptr<std::thread> thread_ptr
        {new std::thread{&thread_pool::default_routine, this}};
        _threads.push_back(thread_ptr);
    }
}

//to each thread
void thread_pool::default_routine(){
    while(_running){
        std::unique_lock<std::mutex> u_lock{_mutex, std::defer_lock};
        u_lock.lock();
        //no task so wait
        while(!has_task()){
            _has_task_cv.wait(_unique_lock);//unlock and block
        }
        //someone added a task to the tasks
        // std::cout << "starting to take one task" << std::endl;
        task t{};
        try{
            t = _tasks.front();
        }catch(std::system_error& e){
            u_lock.unlock();
            LOG(ERROR) << e.what();
        }
        _tasks.pop_front();
        _deque_full_cv.notify_one();
        u_lock.unlock();

        t();
    }
}