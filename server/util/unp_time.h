/*************************************************************************
	> File Name: time.h
	> Author: WANG Jiaming
	> Mail: 
	> Created Time: Sun 07 Oct 2018 08:31:03 PM CST
 ************************************************************************/

#ifndef _UNP_TIME_H
#define _UNP_TIME_H

#include <stdint.h>
#include <time.h>
#include <chrono>
#include "XString.h"
#include <sys/time.h>

using namespace std::chrono;

namespace util{
class Time{
public:
    Time(int64_t microSecondsSinceEpoch) 
        : m_microSecondsSinceEpoch(microSecondsSinceEpoch){}
    template <typename DURATION>
    explicit Time(const time_point<DURATION>& tp){
        from_chrono_time_point(tp);
    }
    int64_t microSecondsSinceEpoch() const {return m_microSecondsSinceEpoch;}
    time_t secondsSinceEpoch() const{
        return static_cast<time_t>(m_microSecondsSinceEpoch / 1000*1000);
    }
    string toString() const;
public:
    static Time now();
    static Time from_time_t(time_t t){
        return from_time_t(t, 0);
    }
    static Time from_time_t(time_t t, int microseconds){
        return Time(static_cast<int64_t>(t) * 1000 * 1000 + microseconds);
    }
    template <typename DURATION>
    void from_chrono_time_point(const time_point<system_clock, DURATION>& t){
        DURATION time_since_epoch = t.time_since_epoch();
        microseconds micro_seconds_since_epoch = duration_cast<microseconds>(time_since_epoch);
        this->m_microSecondsSinceEpoch = micro_seconds_since_epoch.count();
    }

    template <typename DURATION>
    time_point<DURATION> to_chrono_time_point() const{
        microseconds ms{this->m_microSecondsSinceEpoch};
        auto d = duration_cast<DURATION>(ms);
        return time_point<DURATION>{d};
    }
private:
    int64_t m_microSecondsSinceEpoch;
};

template<typename DURATION>
timeval duration_to_timeval(const DURATION& dura){
    seconds s = duration_cast<seconds>(dura);
    microseconds ms = duration_cast<microseconds>(dura);
    timeval ret;
    ret.tv_sec = s.count();
    ret.tv_usec = ms.count() - duration_cast<microseconds>(s).count();
    return ret;
}

template <typename DURATION>
DURATION timeval_to_duration(const timeval& time_val){
    microseconds us{time_val.tv_sec * 1000 * 1000 + time_val.tv_usec};
    return duration_cast<DURATION>(us);
}
}

#endif
