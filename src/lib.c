#ifndef _LIB_C_
#define _LIB_C_
#include "unp.h"

ssize_t readn(int filedes, void *buff, size_t nbytes);
ssize_t writen(int filedes, const void *buff, size_t nbytes);
ssize_t readline(int filedes, void *buff, size_t maxlen);
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
			a signal before any data was read*/{
				err_msg("nread error: %s\n", strerror(errno));
				nread = 0;
			}
			else
				return -1;
		}else if(nread == 0)
			break;
		nleft -= nread;
		ptr += nread;
	}
	return nbytes - nleft;
}

ssize_t writen(int filedes, const void *buff, size_t nbytes){
	size_t 	nleft;
	ssize_t nwrite;
	const char 	*ptr;
	
	ptr = buff;
	nleft = nbytes;

	while(nleft > 0){
		if((nwrite = write(filedes, ptr, nleft)) < 0){
			if(nwrite < 0 && errno == EINTR){
				err_msg("nwrite error: %s\n", strerror(errno));
				nwrite = 0;
			}
			else
				return -1;
		}
		nleft -= nwrite;
		ptr += nwrite;
	}
	return nbytes;
}

ssize_t readline(int fd, void *vptr/*out*/, size_t maxlen){
	ssize_t 	n, rc;
	char 		c, *ptr;

	ptr = vptr;
	for(n = 1; n < maxlen; n++){
	again:
		if((rc = read(fd, &c, 1)) == 1){
			*ptr++ = c;
			//*ptr = c;ptr++;
			if(c == '\n')
				break;
		}else if(rc == 0){
			*ptr = 0;
			return n-1;
		}else{
			if(errno == EINTR)
				goto again;
			return -1;
		}
	}
	*ptr = 0;
	return n;
}
#endif