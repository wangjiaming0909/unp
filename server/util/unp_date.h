#ifndef _UNP_DATE_H_
#define _UNP_DATE_H_
#include <sys/time.h>
#include "XString.h"

namespace util{

class Date{
public:
    struct YearMonthDay{
        int year; int month; int day;
    };
    Date(int y, int m, int d);
    explicit Date(const struct tm&);
    string toString() const ;
    YearMonthDay yearMonthDay()const;
    int Date::get_year() const{ return yearMonthDay().year; }
    int Date::get_month() const{ return yearMonthDay().month; }
    int Date::get_day() const{ return yearMonthDay().day; }
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
#endif
