#ifndef _HEADERS_H_
#define _HEADERS_H_
#include "unp.h"

ssize_t readn(int filedes, void *buff, size_t nbytes);
ssize_t writen(int filedes, const void *buff, size_t nbytes);
ssize_t readline(int filedes, void *buff, size_t maxlen);
ssize_t readn(int fd, void *vptr, size_t nbytes);
void err_ret(const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);
void err_sys(const char *fmt, ...);
int inet_pton_loose(int family, const char *strptr/*in*/, void *addrptr/*out*/);

#endif
