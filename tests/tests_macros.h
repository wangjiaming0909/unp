#ifndef _TESTS_MACROS_H_
#define _TESTS_MACROS_H_

#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <cassert>

#ifndef CONSOLE_LOG
#define CONSOLE_LOG(message) std::cout << message << std::endl;
#endif

#ifndef ASSERT_OK
#define ASSERT_OK std::cout << std::left << "--" << std::setw(60) << std::setfill('-')<< __func__ << std::right << std::setw(4) << "OK" << std::endl;
#endif

#ifndef ASSERT_FAILED
#define ASSERT_FAILED std::cout << std::left << "--" << std::setw(60) << __func__ << std::right << std::setw(4) << "FAILED" << std::endl;
#endif

#ifdef assert
#undef assert
#endif
//! need to solve the problem that have duplicated output of failed case
#define assert(expression)\
if(!(expression))\
    ASSERT_FAILED;

#endif // _TESTS_MACROS_H_
