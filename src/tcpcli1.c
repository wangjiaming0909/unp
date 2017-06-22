#include "unp.h"

void str_cli01(FILE *fp, int sockfd){
    char sendline[MAXLINE], recvline[MAXLINE];
    while(fgets(sendline, MAXLINE, fp) != NULL){

        writen(sockfd, sendline, strlen(sendline));
        if(readline(sockfd, recvline, MAXLINE) == 0)
            err_quit("str_cli: server terminated prematurely: %s", strerror(errno));
        fputs(recvline, stdout);
    }
}

void str_cli(FILE *fp, int sockfd){
    int             maxfdp1;
    fd_set          readset;
    int             ttyiofd, ret;
    char sendline[MAXLINE], recvline[MAXLINE];

    ttyiofd = fileno(fp);
    for(;;){
        FD_ZERO(&readset);
        FD_SET(sockfd, &readset);
        FD_SET(ttyiofd, &readset);
        maxfdp1 = max(sockfd, ttyiofd) + 1;
        if((ret = select(maxfdp1, &readset, NULL, NULL, NULL)) > 0){
            if(FD_ISSET(sockfd, &readset)){//套接字准备好读
                //readline return 0； 收到EOF，由于服务端进程已经发送了FIN, 因此客户端认为服务端不会再给自己发送数据，read直接返回
                if(readline(sockfd, recvline, MAXLINE) == 0){
                    err_quit("readline return 0: %s", strerror(errno));
                }
                fputs(recvline, stdout);
            }
            if(FD_ISSET(ttyiofd, &readset)){//终端准备好读
                if(fgets(sendline, MAXLINE, fp) == NULL)
                    return;
                writen(sockfd, MAXLINE, fp);  
                // if((fgets(sendline, MAXLINE, fp)) != NULL){
                //     writen(sockfd, sendline, strlen(sendline));
                // }else{//键入ctrl + D, fgets返回NULL，直接返回
                //     return ;
                // }
            }
        }
        // else if(ret == -1 && errno == EINTR){
        //     err_quit("interrupted by signal INTR: %s", strerror(errno));
        // }
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