#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
//#include <event.h>

using namespace std;

int main02(){
//    event_base *base = event_base_new();
    sockaddr_in     servaddr;
    int             sockfd, n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        cout << "socket error " << endl;
        cout << strerror(errno) << endl;
        exit(-1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htobe16(80);
    n = inet_pton(AF_INET, "192.168.0.21", &servaddr.sin_addr);
    if(n == -1){
        cout << "inet_pton error.." << endl;
        cout << strerror(errno) << endl;
        exit(-1);
    }
    while(1){
    n = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(n == -1){
        cout << "connect error" << endl;
        cout << strerror(errno) << endl;
//        exit(-1);
    }else break;
    }
    while(1){
        sleep(1);
        n = write(sockfd, "wangjiaming", 12);
    }
    cout << "over...." << endl;
    return 0;
}
