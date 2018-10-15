#ifndef _UNP_THREAD_BASE_H_
#define _UNP_THREAD_BASE_H_

#include "../util/XString.h"
#include "pmutex.h"
#include "stdmutex.h"
#include "pcondition.h"
#include "latch.h"
#include <boost/function.hpp>
#include <boost/io/ios_state.hpp>
#include <pthread.h>
#include <iostream>
#include "thread_data_base.h"
#include <boost/bind.hpp>

namespace thread {

static void* thread_proxy(void* params){
    thread_data_sptr thread_info = static_cast<thread_data_base*>(params)->shared_from_this();
    thread_info->self.reset();

    // try{
        thread_info->run();//TODO try catch
    // }catch(exception const &){

    // }
    thread_info->done = true;
    return 0;
}

template <typename Func>
class thread_data : public thread_data_base{
public:
    thread_data(thread_data&) = delete;
    thread_data(const thread_data&) = delete;
    thread_data& operator=(const thread_data&) = delete;
    thread_data(const Func&& f_) : m_f(boost::forward<Func>(f_)){}
//    thread_data(void(*f)()) : 
    void run(){m_f();}
    ~thread_data(){}
private:
    Func m_f;
};


class Thread{
public:
    Thread() noexcept = default;
    Thread(Thread&) = delete;
    Thread(const Thread&) = delete;
    template <typename Func>
    explicit Thread(Func&& f_) : 
        thread_info(make_thread_data(std::forward<Func>(f_))) {
        start_thread();
    }
    template <typename Func, typename A>
    explicit Thread(Func && f_, A& a_)
    : thread_info(make_thread_data(boost::bind(void(), f_, a_))) {
        start_thread();
    }
    template <typename Func, typename A, typename B>
    explicit Thread(Func && f_, A& a_, B& b_)
    : thread_info(make_thread_data(boost::bind(void(), f_, a_, b_))){
        start_thread();
    }
    template <typename Func, typename A, typename B, typename C>
    explicit Thread(Func && f_, A& a_, B& b_, C& c_)
    : thread_info(make_thread_data(boost::bind(void(), f_, a_, b_, c_))){
        start_thread();
    }
    // template <typename Func, typename... _Args>
    // explicit Thread(Func& __f, _Args&... __args) : 
    //     thread_info(){
    // }

    void start_thread(){//TODO exception handling
       thread_info->self = thread_info;
       const int res = pthread_create(&thread_info->thread_id, 0, &thread_proxy, thread_info.get());
       if(res != 0){
           thread_info->self.reset();
        //    return false;
       }
    //    return true;
    }

public:
    template <typename Func>
    static inline thread_data_sptr make_thread_data(const Func&& f_){
        return thread_data_sptr(new thread_data<typename boost::remove_reference<Func>::type>(
            std::forward<Func>(f_)));
    }
    //template <typename Func, typename A>
    //static inline thread_data_sptr make_thread_data(const Func&& f_, A& a_){
    //    return thread_data_sptr(new thread_data<typename boost)
    //}
    // template <typename Func, typename... Args>
    // static inline thread_data_sptr make_thread_data(const Func&& f_, Args&&... args_){
    //     return thread_data_sptr{new thread_data<>()}
    // }

private:
    thread_data_sptr thread_info;

    class id{
        pthread_t m_thread_id;
    public:
        id() noexcept : m_thread_id(0){}
        // explicit id(pthread_t __id) : m_thread_id(__id){}
        template <typename charT, typename traits>
        friend std::basic_ostream<charT, traits>& operator<(
        std::basic_ostream<charT, traits>&os, const id& _thread_id){
            if(_thread_id.m_thread_id){
                boost::io::ios_flags_saver ifs(os);
                return os << std::hex << _thread_id.m_thread_id;
            }else{
                return os << "{Not-any-thread}";
            }
        }
    };
};
} // thread

#endif // _UNP_THREAD_BASE_H_
