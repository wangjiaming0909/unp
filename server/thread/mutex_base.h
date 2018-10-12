#ifndef _MUTEXBASE_H_
#define _MUTEXBASE_H_

namespace thread{

template <typename THREAD_ID_TYPE, typename MUTEX_IMP_TYPE>
class mutex_base{
public:
    mutex_base() : m_holder_thread(0){}
    virtual ~mutex_base(){}
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual MUTEX_IMP_TYPE* get_imp_mutex() = 0;
protected:
    virtual void assign_holder() = 0;
    virtual void unassign_holder() = 0;
protected:
    THREAD_ID_TYPE      m_holder_thread;
    MUTEX_IMP_TYPE      m_mutex_imp;
};

template <typename THREAD_ID_TYPE, typename MUTEX_IMP_TYPE>
class mutex_guard{
public:
    typedef mutex_base<THREAD_ID_TYPE, MUTEX_IMP_TYPE> MutexType;
    explicit mutex_guard(MutexType& mutex) : m_mutex(mutex){
        m_mutex->lock();
    }
    ~mutex_guard(){m_mutex->unlock();}
private:
    MutexType* m_mutex;
};

}
#endif