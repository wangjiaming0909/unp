#include "thread_pool.h"

using namespace thread;

thread_pool::thread_pool(size_t number_of_threads)
    : _n_of_threads(number_of_threads),
    _running(false),
    _mutex(),
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
    // LOG(INFO) << "adding task";
    // log_with_thread_id("trying to get the mutex");
    lock_gd guard(_mutex, std::defer_lock);
    guard.lock();
    // LOG(INFO) << "adding task";
    // log_with_thread_id("get the mutex");
    //put the check inside the lock??
    while(deque_full()){
        _deque_full_cv.wait(guard);
    }
    //if has space for the new task
    _tasks.push_back(t);
    //if the deque is empty, we need to notify one thread when we add one task
    _has_task_cv.notify_one();
    // LOG(INFO) << "now have: " << _tasks.size() << " tasks...";
    // log_with_thread_id("release the mutex");
    guard.unlock();
    // LOG(INFO) << "added task";
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
            if(t->joinable()) 
                t->join();
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
        if(t->joinable()) 
            t->detach();
    }
}

//只要任何一个线程cancel失败，ret就不为0
int thread_pool::cancel(){
    int ret = 0;
    if(_running) {
        pthread_t id;
        for(auto& t : _threads) {
            id = t->native_handle();
            ret |= pthread_cancel(id);
        }
    }
    if(_running) {
        for(auto& t : _threads) {
            if(t->joinable()){
                t->join();
            }
        }
    }
    return ret == 0;
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
        lock_gd guard(_mutex, std::defer_lock);
        pthread_cleanup_push(cancel_cleanup, static_cast<void*>(&guard));
        guard.lock();
        //no task so wait
        while(!has_task()){
            _has_task_cv.wait(guard);//unlock and block
        }

        //this_thread could be canceled here(after you locked the mutex, before you get one task out)
        //if we got canceled, we will invoke the cleanup handler, unlock the mutex
        //using disable cancellability to ensure that 要么取出task并执行完毕，要么不取任何task
        disable_cancellability();
        //someone added a task to the tasks
        task t{};
        try{
            t = _tasks.front();
        }catch(std::system_error& e){
            guard.unlock();
            LOG(ERROR) << e.what();
        }
        _tasks.pop_front();
        // LOG(INFO) << "now have: " << _tasks.size() << " tasks...";
        _deque_full_cv.notify_one();
        guard.unlock();

        t();
        enable_cancellability_and_test();
        pthread_cleanup_pop(1);
    }
}


int thread_pool::disable_cancellability(){
    int cancel_state = 0;
    int ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_state);
    if(ret != 0) {
        LOG(ERROR) << "pthread_setcancelstate error" << strerror(errno);
        cancel_state = -1;
    }
    return cancel_state;
}

void thread_pool::enable_cancellability_and_test(){
    int cancel_state = 0;
    int ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &cancel_state);
    if(ret != 0) {
        LOG(ERROR) << "pthread_setcancelstate error" << strerror(errno);
        return;
    }
    pthread_testcancel();
}

void cancel_cleanup(void* guard) {
    if(guard == 0) return;
    auto g = static_cast<thread_pool::lock_gd*>(guard);
    if(g->owns_lock()) {
        LOG(INFO) << "owns the mutex trying to unlock it...";
        g->unlock();
    }
}
