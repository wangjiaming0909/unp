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

	__STD_TYPE __SSIZE_T_TYPE __ssize_t; --->typedef int __ssize_t
	# define __STD_TYPE		typedef
	#define __SSIZE_T_TYPE		__SWORD_TYPE
	# define __SWORD_TYPE		int
* * */
ssize_t readn(int fd, void *vptr, size_t nbytes){
    size_t      nleft;//size_t long unsigned int
    ssize_t     nread;	
	char	 	*ptr;
	
	ptr = vptr;
	nleft = nbytes;
	while(nleft > 0){
		if((nread = read(fd, ptr, nleft)) < 0){
			if(errno == EINTR)/*EINTR, the call was interrupted by \
			a signal before any data was read*/
				err_msg("nread error: %s\n", strerror(errno));
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
	size_t 	nleft;
	ssize_t nwrite;
	char 	*ptr;
	
	ptr = buff;
	nleft = nbytes;

	while(nleft > 0){
		if((nwrite = write(filedes, ptr, nleft)) < 0){
			if(nwrite < 0 && errno == EINTR)
				err_msg("nwrite error: %s\n", strerror(errno));
				nwrite = 0;
			else
				return -1;
		}
		nleft -= nwrite;
		ptr += nwrite;
	}
	return n;
}

ssize_t readline(int fd, void *vptr, size_t maxlen){

}