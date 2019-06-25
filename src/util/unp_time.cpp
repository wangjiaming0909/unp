/*************************************************************************
	> File Name: time.cpp
	> Author: 
	> Mail: 
	> Created Time: Sun 07 Oct 2018 08:39:04 PM CST
 ************************************************************************/

#include "unp_time.h"
#include <stdio.h>
#include <inttypes.h>

using namespace util;

string Time::toString() const
{
    char buf[32] = {0};
    int64_t seconds = m_microSecondsSinceEpoch / (1000*1000);
    int64_t microseconds = m_microSecondsSinceEpoch % (1000*1000);
    snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

Time Time::now()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = tv.tv_sec;
    return Time(seconds * 1000*1000 + tv.tv_usec);
}


