
#include "unp.h"
#include "handle_set.h"
#include "../util/unp_time.h"
#include <time.h>
#include <poll.h>
#include <sys/socket.h>
#include "../util/easylogging++.h"



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

int unp::handle_timed_connect_using_poll(int handle, milliseconds* timeout){
    struct pollfd fds;
    fds.fd = handle;
    fds.events = POLLIN | POLLOUT;//in and out events both need to listen
    fds.events = 0;
    //timeout is nullptr means waiting infinitely, so the timeout patameter passed to poll is -1
    //else we will pass timeout.count(), milliseconds to wait
    int n = ::poll(&fds, 1, timeout ? timeout->count() : -1);
    if(n <= 0){//poll failed or time out
        if(n == 0 && timeout && timeout->count() > 0) errno = ETIMEDOUT;
        return INVALID_HANDLE;
    }
    //success n > 0
    //but n > 0 does not means that connect succeed, 
    //connect failed will return n > 0
    //need to use getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
    //when connect failed, the revent could be POLLIN or POLLERR
    int sock_err = 0;
    socklen_t sock_err_len = sizeof sock_err;
    int ret = 0;
    if(fds.revents & (POLLIN | POLLERR)){
        ret = ::getsockopt(handle, SOL_SOCKET, SO_ERROR, &sock_err, &sock_err_len);
        if(ret < 0) handle = INVALID_HANDLE;
        if(sock_err != 0){//error occured
            errno = sock_err;
            handle = INVALID_HANDLE;
        }
    }
    return handle;
}

int unp::handle_timed_connect_using_select(int handle, milliseconds* timeout){
    handle_set read_handles, write_handles;
    write_handles.set_bit(handle);
    read_handles.set_bit(handle);
   timeval timeout_timeval = util::duration_to_timeval(*timeout);
    int n = ::select(handle + 1, read_handles.get_fd_set(), write_handles.get_fd_set(), 0, &timeout_timeval);
    if(n == 0 && timeout && timeout->count() > 0){
        errno = ETIMEDOUT;
        return INVALID_HANDLE;
    }
    //TODO check the fds

    int sock_err = 0;
    socklen_t sock_err_len = sizeof sock_err;
    int ret = 0;
    if(FD_ISSET(handle, write_handles.get_fd_set()) || FD_ISSET(handle, read_handles.get_fd_set())){
        ret = ::getsockopt(handle, SOL_SOCKET, SO_ERROR, &sock_err, &sock_err_len);
        if(ret < 0) handle = INVALID_HANDLE;
        if(sock_err != 0){
            errno = sock_err;
            handle = INVALID_HANDLE;
        }
    }
    return handle;
}

int unp::handle_timed_accept_using_poll(
        int listen_handle, 
        const milliseconds* timeout, 
        bool restart){
    struct pollfd fds;
    fds.fd = listen_handle;
    fds.events = POLLIN;
    fds.revents = 0;

    for(;;){//we need a for if restart is enabled
        //timeout == nullptr means that wait infinitely, 
        //timeout.count() == 0 means do not wait
        int n = ::poll(&fds, 1, timeout ? timeout->count() : -1);
        switch(n){
            case -1:
                //interrupt by signal, restart the poll
                if(errno == EINTR && restart) continue;
                else  return -1;// other errors
            case 0: //timeout
                //do not wait, but we got 0 pollfd
                if(timeout && timeout->count() == 0) errno = EWOULDBLOCK;
                //wait for timeout->count(), and returned 0, so ETIMEOUT
                if(timeout && timeout->count() > 0) errno = ETIMEDOUT;
                return -1;
            case 1://we only have one fd in pollfd
                return 0;//means that one connection come, we can accept now, and the accept won't block
            default:
                errno = EINVAL;
                return -1;
        }
    }
}
