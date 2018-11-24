#ifndef _UNP_HANDLE_SET_H_
#define _UNP_HANDLE_SET_H_
#include <sys/types.h>
#include <string.h>
#include "macros.h"


namespace unp{

class handle_set{
public:
    handle_set(){ reset(); }
    ~handle_set(){}
    handle_set(const fd_set &set){
        reset();
        memcpy(&set_, &set, sizeof set_);
    }
    void reset(){
        max_handle_ = INVALID_HANDLE;
        size_ = 0;
        FD_ZERO(&set_);
    }
    bool is_set(int fd) const {
        //why need cast ---ace
        // fd_set * set = const_cast<fd_set*>(&this->set_);
        return FD_ISSET(fd, &set_);
    }
    void set_bit(int fd){
        //check the fd and if it has been set in set_
        if(fd != INVALID_HANDLE && !is_set(fd)){
            FD_SET(fd, &set_);
            ++size_;
            if(fd > max_handle_)//set the max_fd
                max_handle_ = fd;
        }
    }
    int nums_set() const{return size_;}
    fd_set* get_fd_set(){ 
        if(size_ > 0) return &set_;
        else return nullptr;
    }
    //not const, and not return const
    //select will change the fd_set when get the fd_set*
    fd_set* get_select_fd_set_ptr(){
        if(size_ > 0) 
            return &set_;
        return static_cast<fd_set*>(0);
    }
private:
    int         size_;
    int         max_handle_;
    fd_set      set_;
};



}
#endif // _UNP_HANDLE_SET_H_