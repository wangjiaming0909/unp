#ifndef _UNP_H_
#define _UNP_H_
#include <chrono>

#define INVALID_HANDLER -1


namespace unp{

int handle_ready_select(
    int handle,
    std::chrono::microseconds& timeout,
    int read_ready,
    int write_ready,
    int exception_ready);

}

#endif // _UNP_H_