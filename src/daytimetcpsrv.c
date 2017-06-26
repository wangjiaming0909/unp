#include "unp.h"
#include <time.h>
#include "headers.h"

int
main03(int argc, char **argv){
    int                 listenfd, connfd;
    struct sockaddr_in  servaddr;
    char                buff[MAXLINE];
    time_t              ticks;
    unsigned int        ret, addr_int;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = inet_pton(AF_INET, "127.0.0.1", &addr_int);
    servaddr.sin_addr.s_addr = addr_int;
    
    servaddr.sin_port = htons(1013);

    bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    for(;;){
        connfd = accept(listenfd, (SA *)NULL, NULL);

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));

        close(connfd);
    }
    return 0;
}

