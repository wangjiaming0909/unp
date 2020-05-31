#ifndef _UTIL_TIMER_H_
#define _UTIL_TIMER_H_
#include <boost/timer/timer.hpp>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

namespace utils{
class timer{
public:
    timer(const string s) : timer_(), s_(s){

    }
    ~timer() {
        cout
            << std::left << std::setw(48) << s_
            << std::left << std::setw(24) << std::fixed << timer_.format()
            << endl;
    }

private:
    boost::timer::cpu_timer timer_;
    string s_;
};
}

#endif
