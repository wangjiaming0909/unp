#include "unp.h"

void str_cli(FILE *fp, int sockfd){
    char sendline[MAXLINE], recvline[MAXLINE];
    while(fgets(sendline, MAXLINE, fp) != NULL){

        writen(sockfd, sendline, strlen(sendline));
        if(readline(sockfd, recvline, MAXLINE) == 0)
            err_quit("str_cli: server terminated prematurely: %s", strerror(errno));
        fputs(recvline, stdout);
    }
}
int main(int argc, char **argv){
    int sockfd;    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1001);
    inet_pton(AF_INET, "139.224.239.147", &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    err_msg("connect success...");
    str_cli(stdin, sockfd);
    exit(0);
}

int maintcpcli05(int argc, char **argv){
    int i, sockfd[5];
    struct sockaddr_in servaddr;

    for(i = 0; i < 5; i++){
        sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(1001);
        inet_pton(AF_INET, "139.224.239.147", &servaddr.sin_addr);

        connect(sockfd[i], (struct sockaddr *)&servaddr, sizeof(servaddr));
    }
    str_cli(stdin, sockfd[0]);
    exit(0);
}