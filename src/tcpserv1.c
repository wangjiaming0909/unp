#include "unp.h"


int main(int argc, char *argv[]){
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
    servaddr.sin_port = htons(SERV_PORT);

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
        if((child_pid = fork()) == 0){
            close(listenfd);
            err_msg("child process called..");
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
    return 0;
}