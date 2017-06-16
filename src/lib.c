/*************************************************************************
	> File Name: lib.c
	> Author: 
	> Mail: 
	> Created Time: Fri 16 Jun 2017 03:24:18 PM CST
 ************************************************************************/

#include "unp.h"

ssize_t readn(int filedes, void *buff, size_t nbytes);
ssize_t writen(int filedes, const void *buff, size_t nbytes);
size_t readline(int filedes, void *buff, size_t maxlen);
/*
* ssize_t的宏定义:
* #ifndef __ssize_t_defined
* typedef __ssize_t ssize_t;
* #define __size_t_defined
* #endif
* * */
ssize_t readn(int fd, void *vptr, size_t n){
    size_t      nleft;
    ssize_t     nread;
	char	 	*ptr;
	
	ptr = vptr;
	nleft = n;
	while(nleft > 0){
		if((nread = read(fd, ptr, nleft)) < 0){
			if(errno == EINTR)/*EINTR, the call was interrupted by a signal before any data was read*/
				nread = 0;
			else
				return -1;
		}else if(nread == 0)
			break;
		nleft -= nread;
		ptr += nread;
	}
	return n - nleft;
}

ssize_t writen(int filedes, const void *buff, size_t nbytes){
	
}
