#include "unp.h"
#include "headers.h"

int mainudp(int argc, char **argv){
    int     listenfd, connfd, udpfd, nready, maxfdp1;
    char    mesg[MAXLINE];
    pid_t   childpid;
    fd_set  rset;
    ssize_t n;
    socklen_t len;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;
    void sig_chld(int);

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_quit("socket error");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1001);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    listen(listenfd, LISTENQ);
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1001);

    bind(udpfd, (SA*)&servaddr, sizeof(servaddr));

    signal(SIGCHLD, sig_chld);
    FD_ZERO(&rset);
    maxfdp1 = max(listenfd, udpfd) + 1;
    for(;;){
    	FD_SET(udpfd, &rset);
    	FD_SET(udpfd, &rset);
    	if((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0){
    		if(errno == EINTR)
    			continue;
    		else
    			err_quit("select error");
    	}
    	if(FD_ISSET(listenfd, &rset)){
    		len = sizeof(cliaddr);
    		connfd = accept(listenfd, (SA*)&cliaddr, &len);
    	}
    }
}
