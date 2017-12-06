#include "unp.h"
#include "headers.h"

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

//SIG_CHLD信号的处理函数
/*问题：当高并发时，可能存在多个套接字同时发送FIN，服务器收到第一个FIN时，其对应的子进程终止,
    父进程接收到SIGCHLD信号，调用sig_chld函数， 同时其他几个子进程也收到FIN，
    同时发出SIGCHLD信号，此时，屏蔽了SIGCHLD信号，因此其他几个SIGCHLD信号处于未决状态，由于
    UNIX信号一般是不排队的，因此，最后可能执行完第一个sig_chld之后其他的信号合并为一个，
    也就是存在有的信号终止的子进程不能被wait，成为僵尸进程。
*/
void sig_chld01(int signo){
    pid_t   pid;//子进程pid
    int     stat;//子进程退出状态

    pid = wait(&stat);
    err_msg("child %d terminated", pid);
    return ;
}

//SIG_CHLD信号处理函数， 立即返回不阻塞
/*改进： 不使用wait， 使用waitpid函数*/
void sig_chld(int signo){
    pid_t   pid;
    int     stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0){/*小于0表示出错， 等于0表示没有等到子进程*/
        printf("child %d teminated\n", pid);
    }
    return ;
}
