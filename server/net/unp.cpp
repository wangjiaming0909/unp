
#include "unp.h"
#include "handle_set.h"
#include "../util/unp_time.h"
#include <time.h>
#include <poll.h>



//this function is used to select on one handle and with the event specified
int unp::handle_ready_using_select(
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
    
    switch (result) {
        case 0:  // Timer expired.
            errno = ETIME;
            /* FALLTHRU */
        case -1: // we got here directly - select() returned -1.
            return -1;
        case 1: // Handle has data.
        /* FALLTHRU */
        default: // default is case result > 0; return a
            return result;
    }
}

int unp::handle_ready_using_poll(
    int handle, 
    const std::chrono::milliseconds& timeout,
    int read_ready,
    int write_ready){
    struct pollfd fds;
    fds.fd = handle;
    fds.events = read_ready ? POLLIN : 0;
    if(write_ready)
        fds.events |= POLLOUT;
    fds.revents = 0;//actual events that happened
    int const result = ::poll(&fds, 1, timeout.count());
    switch (result) {
        case 0:  // Timer expired.
            errno = ETIME;
            /* FALLTHRU */
        case -1: // we got here directly - select() returned -1.
            return -1;
        case 1: // Handle has data.
        /* FALLTHRU */
        default: // default is case result > 0; return a
            return result;
    }
}