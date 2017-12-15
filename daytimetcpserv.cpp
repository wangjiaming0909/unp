#include <iostream>
#include <netinet/in.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include "unp.h"
using namespace std;

int main(){
    int                  listenfd, connfd, ret;
    socklen_t            len;
    struct sockaddr_in   servaddr, cliaddr;
    char                 buf[MAXLINE];
//    time_t               ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(!listenfd){
        cout << "f socekt error.." << endl;
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(ret != 0){
        cout << "f bind error.." << endl;
        cout << errno << " " << strerror(errno) << endl;
        exit(0);
    }

    Listen(listenfd, LISTENQ);
    if(ret != 0){
        cout << "f listen error.." << endl;
        cout << errno << " " << strerror(errno) << endl;
        exit(0);
    }
    for(; ; ){
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
        cout << "connect from " <<
                inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)) <<
                ", port " << ntohs(cliaddr.sin_port) << endl;
//        ticks = time(NULL);

    }
    cout << 123 << endl;
    return ret;
}
