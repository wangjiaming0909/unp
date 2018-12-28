#ifndef _PTHREAD_POOL_H_
#define _PTHREAD_POOL_H_
#include <functional>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include <assert.h>
#include <iostream>
#include "server/util/easylogging++.h"

namespace thread {
class thread_pool{
public:
    typedef std::function<void ()> task;
    using lock_gd = std::lock_guard<std::mutex>;
    using micro_seconds = std::chrono::microseconds;
    thread_pool(size_t number_of_threads)
        : _n_of_threads(number_of_threads),
        _running(false),
        _mutex(),
        _unique_lock(_mutex, std::defer_lock),
        _has_task_cv(),
        _deque_full_cv(),
        _tasks(),
        _threads(0){
    }
    ~thread_pool(){
        if(_running)
        stop(micro_seconds(0));
    }
    //will we have multi threads to add_tasks?
    void add_task(const task& t){
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
    void start(){
        _running = true;
        initialize();
    }

    //? if timeout == nullptr -> wait infinitly, and all the threads will not close
    //? if timeout != nullptr, but the timeout is 0 -> do not wait, close all the threads
    //? if timeout != nullptr, and timeout is not 0 -> wait for the timeout, and stop them all
    void wait(const micro_seconds* timeout) {
        if(timeout == nullptr) {
            for(auto &t : _threads) {
                if(t->joinable()) t->join();
            }
        } else {
            stop(*timeout);
        }
    }


    void stop(const micro_seconds& timeout){
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
private:
    //allocate threads
    void initialize(){
        for(size_t i = 0; i < _n_of_threads; i++){
            //construct the new thread, let it go the default routine
            std::shared_ptr<std::thread> thread_ptr
            {new std::thread{&thread_pool::default_routine, this}};
            _threads.push_back(thread_ptr);
        }
    }
    //only invoked in the add_task 
    //so it will always be locked
    inline bool has_task(){
        return !_tasks.empty();
    }
    inline bool deque_full(){
        return _tasks.size() > _n_of_threads;
    }
    //to each thread
    void default_routine(){
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
private:
    size_t                                          _n_of_threads;
    bool                                            _running;
    std::mutex                                      _mutex;
    std::unique_lock<std::mutex>                    _unique_lock;
    std::condition_variable                         _has_task_cv;
    std::condition_variable                         _deque_full_cv;
    std::deque<task>                                _tasks;
    std::vector<std::shared_ptr<std::thread>>       _threads;
};
}

#endif
