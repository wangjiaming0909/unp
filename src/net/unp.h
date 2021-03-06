#ifndef _UNP_H_
#define _UNP_H_
#include <chrono>
#include <thread/thread_pool.h>

#define INVALID_HANDLE -1
using namespace std::chrono;

namespace unp{
enum class reactor_imp_t_enum
{
    USING_SELECT, USING_POLL, USING_EPOLL
};


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

inline int handle_read_ready_using_select(int handle, microseconds& timeout)
{
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

int handle_timed_connect_using_poll(int handle, milliseconds* timeout);
int handle_timed_connect_using_select(int handle, milliseconds* timeout);
int handle_timed_accept_using_poll(int listen_handle, const milliseconds* timeout, bool restart);

thread::thread_pool* get_thread_pool(int num);
}

#ifdef TESTING
#define TEST_PRIVATE public
#else
#define TEST_PRIVATE private
#endif //TESTING

#ifdef TESTING
#define TEST_PROTECTED public
#else
#define TEST_PROTECTED protected
#endif //TESTING

#define NOT_USED(arg) (void)(arg)
#define THROW_NOT_IMPLETED_EXCEPTION throw new std::logic_error("Not Implemented")

#endif // _UNP_H_
