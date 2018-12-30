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
    thread_pool(size_t number_of_threads);
    ~thread_pool();
    void add_task(const task& t);
    void start();
    void wait(const micro_seconds* timeout);
    void stop(const micro_seconds& timeout);
private:
    //allocate threads
    void initialize();
    //only invoked in the add_task 
    //so it will always be locked
    bool has_task(){ return !_tasks.empty(); }
    bool deque_full(){ return _tasks.size() > _n_of_threads; }
    void default_routine();
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
