#ifndef _UNP_DATE_H_
#define _UNP_DATE_H_
#include <sys/time.h>
#include "XString.h"

namespace util{
namespace date{

class Date{
public:
    struct YearMonthDay{
        int year; int month; int day;
    };
    Date(int y, int m, int d);
    explicit Date(const struct tm&);
    string toString() const ;
    YearMonthDay yearMonthDay()const;
    int year() const;
    int month() const;
    int day() const;
    int julianDayNumber()const{return m_julianDayNumber;}
private:
    int m_julianDayNumber;
};

inline bool operator<(Date x, Date y){
    return x.julianDayNumber() < y.julianDayNumber();
}

inline bool operator==(Date x, Date y){
    return x.julianDayNumber() == y.julianDayNumber();
}

}
}
#endif