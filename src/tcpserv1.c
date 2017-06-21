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
