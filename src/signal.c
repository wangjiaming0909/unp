#include "unp.h"

Sigfunc *signal(int signo, Sigfunc *func){
    struct sigaction act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(signo == SIGALRM){
#ifdef SA_INTERRUPT/*由此信号中断的系统调用不自动重启动*/ 
        act.sa_flags |= SA_INTERRUPT;//Sunos 4.x
#endif
    }else{
#ifdef SA_RESTART/*由此信号中断的系统调用自动重启动*/  
        act.sa_flags |= SA_RESTART;// SVR4 4.4BSD
#endif
    }
    if(sigaction(signo, &act, &oact) < 0)
        return SIG_ERR;
    return oact.sa_handler;//返回旧信号处理程序函数指针
}