/*************************************************************************
	> File Name: tcpserv.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年12月15日 星期五 10时15分27秒
 ************************************************************************/

#include <iostream>
#include "include/unp.h"
using namespace std;

void str_echo(int);
int main(){
    int         listenfd, connfd, ret;
    pid_t       child_pid;
    socklen_t   clilen;
    sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd == -1){
        cout << "error socket ..." << endl;
        cout << strerror(errno) << endl;
        exit(-1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);

    ret = bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    if(ret == -1){
        cout << "error bind ..." << endl;
        cout << strerror(errno) << endl;
        exit(-2);
    }

    ret = listen(listenfd, LISTENQ);
    if(ret == -1){
        cout << "error listen ..." << endl << strerror(errno) << endl;
        exit(-3);
    }

    for(;;){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (sockaddr*)&cliaddr, &clilen);
        if((child_pid = fork()) == 0){
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    
    }
}
