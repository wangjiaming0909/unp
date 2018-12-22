#ifndef _UNP_THREAD_LOCK_GUARD_H_
#define _UNP_THREAD_LOCK_GUARD_H_

namespace thread{
template <typename lock_type>
class lock_guard{
public:
    lock_guard(lock_type& lock) : locker_(&lock){
        this->locker_->lock();
    }
    ~lock_guard(){
        locker_->unlock();
    }

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;
private:
    lock_type*      locker_;
};
}//namespace thread

#endif // _UNP_THREAD_LOCK_GUARD_H_