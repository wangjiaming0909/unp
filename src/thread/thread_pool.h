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
#include "util/easylogging++.h"

/*
    unique_lock,
        std::defer_lock 使用mutex创建一个unique_lock，在初始化时不锁住mutex，m应是一个没有在当前线程锁住的mutex对象
        std::adopt_lock 使用mutex创建一个unique_lock，
            m应是一个已经被当前线程锁住的mutex对象，并且当前创建的unqiue_lock完全掌控该mutex
            adopting 感觉就是完全将该mutex收下
        std::try_to_lock 使用mutex创建一个unique_lock对象，并尝试对mutex上锁(try_lock)
*/

namespace thread {
class thread_pool{
public:
    using task = std::function<void ()>;
    using lock_gd = std::unique_lock<std::mutex>;
    using micro_seconds = std::chrono::microseconds;

public:
    thread_pool(size_t number_of_threads = 0);
    ~thread_pool();
    void add_task(const task& t);
    void start();
    void wait(const micro_seconds* timeout);
    //cancel all the threads immediately
    int cancel();
    void set_num_of_threads(size_t n){_n_of_threads = n;}
private:
    //对当前线程调用
    int disable_cancellability();
    void enable_cancellability_and_test();

    void stop(const micro_seconds& timeout);
    //allocate threads
    void initialize();
    //only invoked in the add_task 
    //so it will always be locked
    bool has_task(){ return !_tasks.empty(); }
    bool deque_full();
    void default_routine();

private:
    size_t                                          _n_of_threads;
    bool                                            _running;
    //此mutex可用于多个线程，主要为了配合cv使用
    //  1，为了保护添加task时可能队列已满以及取task时可能的队列已空
    //  2, 
    std::mutex                                      _mutex;
    std::condition_variable                         _has_task_cv;
    std::condition_variable                         _deque_full_cv;
    std::deque<task>                                _tasks;
    std::vector<std::shared_ptr<std::thread>>       _threads;
};
}

void cancel_cleanup(void* guard);

#endif
