#include <iostream>
#include "include/unp.h"
using namespace std;

int main(){
    int                 sockfd, ret;
    sockaddr_in         servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        cout << "error socket ..." << endl;
        cout << strerror(errno) << endl;
        exit(-1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9877);
    inet_pton(AF_INET, "192.168.0.22", &servaddr.sin_addr);

    ret = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(ret == -1){
        cout << "error connect ..." << endl;
        cout << strerror(errno) << endl;
        exit(-2);
    }
    cout << "connect success" << endl;
    exit(0);

}