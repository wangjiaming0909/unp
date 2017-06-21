#include "unp.h"

void sig_chld(int);

void str_echo(int sockfd){
    err_msg("calling str_echo..: %s", strerror(errno));
    ssize_t n;
    char    buf[MAXLINE];
again:
    while((n = readline(sockfd, buf, MAXLINE))> 0)
        writen(sockfd, buf, n);
    if(n < 0 && errno == EINTR)
        goto again;
    else if(n < 0)
        err_sys("str_echo: read error");
}


/*
    改进版服务器程序可以解决一下三个问题
    1， 当fork子进程时，捕获SIGCHLD信号
    2， 捕获信号好后，处理被中断的系统调用， 父进程正阻塞在accept调用时，接收到SIGCHLD信号，
        accept被中断，因此在处理SIGCHLD信号时，需要给sigaction函数提供SA_RESART标志：act.sa_flags |= SA_RESTART
    3， 使用waitpid避免产生僵尸进程
*/
int maintcpserv(int argc, char *argv[]){
    int         listenfd, connfd, ret;
    pid_t       child_pid;
    socklen_t   clilen;
    struct sockaddr_in servaddr, cliaddr;

    bzero(&servaddr, sizeof(servaddr));
    bzero(&cliaddr, sizeof(cliaddr));

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        err_msg("socket error:%s", strerror(errno));
        exit(-1);
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1001);

    if((ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0){
        err_msg("bind error:%s", strerror(errno));
        exit(-1);
    }
    if((ret = listen(listenfd, LISTENQ)) < 0){
        err_msg("listen error:%s", strerror(errno));
        exit(-1);
    }
    for(;;){
        clilen = sizeof(cliaddr);
        if((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0){
            err_msg("accept error:%s", strerror(errno));
            exit(-1);
        }
        signal(SIGCHLD, sig_chld);
        if((child_pid = fork()) == 0){
            close(listenfd);
            //err_msg("child process called..");
            str_echo(connfd);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
    return 0;
}

/*
    当终止子进程时：
        子进程阻塞与read函数，
        此时kill子进程，关闭所有打开的描述符，向客户端发送一个FIN，进入FIN_WAIT1
        客户端回复一个ACK，客户端进入CLOSE_WAIT， 服务端进入FIN_WAIT2；
        父进程接收到SIGCHLD信号，回收子进程；
        客户端继续向该子进程发送数据，调用write函数，服务端接收到数据，由于之前的套接字已经终止，返回一个RST
        由于子进程发送了FIN，因此客户端认为服务端不会再给自己发送数据，因此read直接返回0， 表示EOF，

*/