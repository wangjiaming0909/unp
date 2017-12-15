/*************************************************************************
	> File Name: lib/str_echo.cpp
	> Author: 
	> Mail: 
	> Created Time: 2017年12月15日 星期五 10时33分09秒
 ************************************************************************/

#include<iostream>
#include "../include/unp.h"
using namespace std;

void str_echo(int sockfd){
    ssize_t     n;
    char        buf[MAXLINE];

again:
    while((n = read(sockfd, buf, MAXLINE)) > 0)
        write(sockfd, buf, n);
    if(n < 0 && errno == EINTR)
        goto again;
    else if(n < 0){
        cout << "error str_echo ..." << endl << strerror(errno) << endl;
        exit(-5);
    }
}
