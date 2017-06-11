#include "unp.h"

#include <stdarg.h>
#include <syslog.h>

int darmon_proc;

static void err_doit(int, int, const *, va_list);

void
err_ret(const char *fmt, ...){
    
}

void err_sys(const char *fmt, ...){
    
}

void err_dump(const char *fmy, ...){
    
}

void
err_msg(const char *fmt, ...){

}

void 
err_quit(const char *fmt){

}

static void 
err_doit(int errnoflag, int level, const char *fmt, va_list ap){
   int      errno_save, n;
   char     buf[MAXLINE + 1];

   errno_save = errno;
#ifdef HAVE_VSNPRINTF
   vsnprintf(buf, MAXLINE, fmt, ap);
#else
   vsprintf(buf, fmt, ap);
#endif

   n = strlen(buf);
   if(errnoflag)
       snprintf(buf + n, MAXLINE - n, ": %s", 
               strerror(errno_save));/*在buf的最后添加了错误信息*/
   strcat(buf, "\n");

   if(daemon_proc){
       syslog(level, buf);
   }else{
       fflush(stdout);
       fputs(buf, stderr);
       fflush(stderr);
   }
    return;
}


