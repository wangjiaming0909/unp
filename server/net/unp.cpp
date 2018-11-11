
#include "unp.h"
#include "handle_set.h"
#include "../util/unp_time.h"
#include <time.h>



int unp::handle_ready_select(
        int handle,
        std::chrono::microseconds& timeout,
        int read_ready,
        int write_ready,
        int exception_ready){
    handle_set set;
    set.set_bit(handle);
    int select_width = handle + 1;
    timeval time_remain = util::duration_to_timeval(timeout);
    int result = ::select(select_width,
                          read_ready ? set.get_fd_set() : 0,
                          write_ready ? set.get_fd_set() : 0,
                          exception_ready ? set.get_fd_set() : 0,
                          &time_remain);
    timeout = util::timeval_to_duration<std::chrono::microseconds>(time_remain);
    return result;
}
