#ifndef _TESTS_MACROS_H_
#define _TESTS_MACROS_H_

#include <iostream>
#include <cassert>
#include <iomanip>

#ifndef CONSOLE_LOG
#define CONSOLE_LOG(message) std::cout << message << std::endl;
#endif

#ifndef ASSERT_OK
#define ASSERT_OK std::cout << std::left << "--" << std::setw(60) << __func__ << std::right << std::setw(4) << "OK" << std::endl
#endif


#endif // _TESTS_MACROS_H_
