#include "unp.h"

int main(int argc, char **argv){
    int     listenfd, connfd, udpfd, nready, maxfdp1;
    char    mesg[MAXLINE];
    pid_t   childpid;
    fd_set  rset;
    ssize_t len;
    socklen_t len;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;
    void sig_chld(int);

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_quit("socket error");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_fmily = AF_INET;
    servaddr.sin_port = htons(1001);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bind(listenfd, LISTENQ);
}
