#ifndef _LIB_C_
#define _LIB_C_
#include "unp.h"
#include "headers.h"

//ssize_t readn(int filedes, void *buff, size_t nbytes);
//ssize_t writen(int filedes, const void *buff, size_t nbytes);
//ssize_t readline(int filedes, void *buff, size_t maxlen);
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

/*-------------------------------readline---------------------------------*/
//Painfully slow version 
ssize_t readline1(int fd, void *vptr/*out*/, size_t maxlen){
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

/*--------------------readline------------------------------------*/

//第三章习题3.3
//inet_pton()
//when family is AF_INET, and strptr is a ipv4 address, return ipv4 address mapped with ipv4
int inet_pton_loose(int family, const char *strptr/*in*/, void *addrptr/*out*/){
    int ret1, ret2;
	struct in_addr inaddr;

    if(AF_INET == family){//ipv4
    	int *addrptrptr = addrptr;
        if((ret1 = inet_pton(family, strptr/*in*/, addrptrptr)) == 0){//invalid ipv4 addr
			err_msg("ipv4, invalid ipv4 address");
			if((ret2 = inet_aton(strptr, &inaddr/*out*/)) != 1){//invoke inet_aton
				err_msg("inet_aton error: %s", strerror(errno));
				return 0;
			}
			*addrptrptr = inaddr.s_addr;
		}else if(ret1 == -1){//error
			err_msg("inet_pton error: %s", strerror(errno));
			return -1;
		}
		
    }else if(AF_INET6 == family){//ipv6
		unsigned char *u6addr = addrptr;
		bzero(u6addr, 16);

		if((ret1 = inet_pton(AF_INET6, strptr, u6addr)) == 0){//invalid ipv6 addr
			err_msg("invalid ipv6 address..");
			if((ret2 = inet_aton(strptr, &inaddr/*out*/)) == 0){//invalid ip address
				err_msg("invalid ip address: %s", strerror(errno));
				return 0;
			}else if(ret2 == 1){
				short int *p1;
				p1 = (void *)u6addr + 10;
				*p1 = -1;
				int *p;
				p = (void *)u6addr + 12;
				*p = inaddr.s_addr;
			}else
				err_msg("inet_aton error: %s", strerror(errno));
				
		}else if(ret1 == -1){//error
			err_msg("inet_pton error: %s", strerror(errno));
			return -1;
		}
    }
    return 0;
}

//return ss_family, can call this function in a child process
int sockfd_to_family(int sockfd){
	struct sockaddr_storage 	ss;
	socklen_t					len;
	
	len = sizeof(ss);
	if(getsockname(sockfd, (SA *)&ss, &len) < 0)
		return -1;
	return ss.ss_family;
}

#endif
