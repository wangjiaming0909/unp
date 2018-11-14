#ifndef _UNP_H_
#define _UNP_H_
#include <chrono>

#define INVALID_HANDLER -1
using namespace std::chrono;

namespace unp{

int handle_ready_using_select(
    int handle,
    microseconds& timeout,
    int read_ready,
    int write_ready,
    int exception_ready);

int handle_ready_using_poll(
    int handle, 
    const milliseconds& timeout,
    int read_ready,
    int write_ready);

inline int handle_read_ready_using_select(int handle, microseconds& timeout){
    return handle_ready_using_select(handle, timeout, 1, 0, 0);
}

inline int handle_write_ready_using_select(int handle, microseconds& timeout){
    return handle_ready_using_select(handle, timeout, 0, 1, 0);
}

inline int handle_exception_ready_using_select(int handle, microseconds& timeout){
    return handle_ready_using_select(handle, timeout, 0, 0, 1);
}

inline int handle_read_ready_using_poll(int handle, const milliseconds& timeout){
    return handle_ready_using_poll(handle, timeout, 1, 0);
}

inline int handle_write_ready_using_poll(int handle, const milliseconds& timeout){
    return handle_ready_using_poll(handle, timeout, 0, 1);
}

inline int handle_timed_complete_using_poll(int handle, milliseconds& timeout);

}

#endif // _UNP_H_