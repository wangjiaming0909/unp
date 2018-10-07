#include "date.h"
#include <cstdio>

namespace util{
namespace date{
int getJulianDayNumber(int year, int month, int day){
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  return day + (153*m + 2) / 5 + y*365 + y/4 - y/100 + y/400 - 32045;
}

struct Date::YearMonthDay getYearMonthDay(int julianDayNumber){
  int a = julianDayNumber + 32044;
  int b = (4 * a + 3) / 146097;
  int c = a - ((b * 146097) / 4);
  int d = (4 * c + 3) / 1461;
  int e = c - ((1461 * d) / 4);
  int m = (5 * e + 2) / 153;
  Date::YearMonthDay ymd;
  ymd.day = e - ((153 * m + 2) / 5) + 1;
  ymd.month = m + 3 - 12 * (m / 10);
  ymd.year = b * 100 + d - 4800 + (m / 10);
  return ymd;
}
}
}

util::date::Date::Date(int y, int m, int d)
    : m_julianDayNumber(date::getJulianDayNumber(y, m, d)){}

util::date::Date::Date(const struct tm& time)
    : m_julianDayNumber(getJulianDayNumber(
        time.tm_year+1900,
        time.tm_mon+1,
        time.tm_mday)){}

util::string util::date::Date::toString()const{
    char buf[32];
    YearMonthDay ymd(yearMonthDay());
    snprintf(buf, sizeof buf, 
        "%4d-%02d-%02d", 
        ymd.year, ymd.month, ymd.day);
    return buf;
}

util::date::Date::YearMonthDay util::date::Date::yearMonthDay() const {
    return getYearMonthDay(m_julianDayNumber);
}