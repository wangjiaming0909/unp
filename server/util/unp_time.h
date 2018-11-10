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
#include "XString.h"

namespace util{
class Time{
public:
    Time(int64_t microSecondsSinceEpoch) 
        : m_microSecondsSinceEpoch(microSecondsSinceEpoch){}
    string toString() const;
    int64_t microSecondsSinceEpoch() const {return m_microSecondsSinceEpoch;}
    time_t secondsSinceEpoch() const{
        return static_cast<time_t>(m_microSecondsSinceEpoch / 1000*1000);
    }
public:
    static Time now();
    static Time fromUnixTime(time_t t){
        return fromUnixTime(t, 0);
    }
    static Time fromUnixTime(time_t t, int microseconds){
        return Time(static_cast<int64_t>(t) * 1000*1000 + microseconds);
    }
private:
    int64_t m_microSecondsSinceEpoch;
};
}

#endif
