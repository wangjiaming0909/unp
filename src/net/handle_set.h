#ifndef _UNP_HANDLE_SET_H_
#define _UNP_HANDLE_SET_H_
#include <sys/types.h>
#include <string.h>
#include "macros.h"
#include "util/easylogging++.h"


namespace unp{

class handle_set{
public:
    handle_set(){ reset(); }
    ~handle_set(){}
    handle_set(const fd_set &set) {
        reset();
        memcpy(&set_, &set, sizeof set_);
    }
    void reset(){
        max_handle_ = INVALID_HANDLE;
        size_ = 0;
        FD_ZERO(&set_);
    }
    bool is_set(int handle) const {
        //why need cast ---ace
        // fd_set * set = const_cast<fd_set*>(&this->set_);
        return FD_ISSET(handle, &set_);
    }
    void set_bit(int handle){
        //check the handle and if it has been set in set_
        if(handle != INVALID_HANDLE && !is_set(handle)){
            FD_SET(handle, &set_);
            ++size_;
            if(handle > max_handle_)//set the max_fd
                max_handle_ = handle;
        }
    }
    void unset_bit(int handle) {
        if(handle != INVALID_HANDLE && handle < FD_SETSIZE && is_set(handle)){
            FD_CLR(handle, &set_);
            --size_;
            if(handle == max_handle_){//if the unseted handle is the maximun handle, need to change the max_handle_
                int tmp_handle = handle + 1;
                while(!is_set(--tmp_handle));
                max_handle_ = tmp_handle;
            }
        }
    }
    int handles_count() const {return size_;}
    fd_set* get_fd_set(){ 
        if(size_ > 0) return &set_;
        else return nullptr;
    }
    //not const, and not return const
    //select will change the fd_set when get the fd_set*
    fd_set* get_select_fd_set_ptr(){
        if(size_ > 0) 
            return &set_;
        return static_cast<fd_set*>(nullptr);
    }
    int first_handle() const {
        //empty fd_set
        if(size_ <= 0) return INVALID_HANDLE;

        int handle = -1;
        while(!is_set(++handle));
        return handle;
    }
    int next_handle(int current_handle) {
        if(size_ == 0) return INVALID_HANDLE;
        if(current_handle != INVALID_HANDLE && !is_set(current_handle)){
            LOG(WARNING) << "the handle is not in the set, handle: " << current_handle;
            LOG(WARNING) << "trying to find the next handle...";
        }
        if(is_last_handle(current_handle)) 
            return INVALID_HANDLE;
        current_handle -= 1;//minus 1
        while(!is_set(++current_handle));//must use ++current_handle, not current_handle++
        return current_handle;
    }

    bool is_last_handle(int handle) const {
        if(handle == max_handle_) return true;
        return false;
    }

private:
    int         size_;
    int         max_handle_;
    fd_set      set_;
};



}
#endif // _UNP_HANDLE_SET_H_