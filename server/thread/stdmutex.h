#ifndef _STD_MUTEX_H_
#define _STD_MUTEX_H_

#include "mutex_base.h"
#include <thread>
namespace thread{
class stdmutex : mutex_base<int, int>{//TODO: not int int
virtual void lock(){
        assign_holder();
    } 
    virtual void unlock(){
        unassign_holder();
    }
    virtual int* get_imp_mutex(){
        return &m_mutex_imp;
    }
protected:
    virtual void assign_holder(){
    }
    virtual void unassign_holder(){
    }
};
}
#endif