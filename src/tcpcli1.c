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

void str_cli02(FILE *fp, int sockfd){
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

void str_cli(FILE *fp, int sockfd){
    int     stdineof, maxfdp1, n;
    fd_set  readset;
    char    buf[MAXLINE];
    
    stdineof = 0;
    FD_ZERO(&readset);
    for(; ; ){
        if(stdineof == 0)//检测该值，若为1， 就不需要再select终端的写入
            FD_SET(fileno(fp), &readset);
        FD_SET(sockfd, &readset);
        maxfdp1 = max(sockfd, fileno(fp)) + 1;
        select(maxfdp1, &readset, NULL, NULL, NULL);
        
        if(FD_ISSET(sockfd, &readset)){//接受了TCP连接的FIN也会导致sockfd可读
            if((n = read(sockfd, (void *)buf, MAXLINE)) == 0){//服务器端发来EOF
                if(stdineof == 1)//客户端先发送的EOF
                    return ;
                else//客户端没有发送EOF， 但是接收到了服务端发来的EOF，说明服务器子进程已经提前终止，提前发来了FIN
                    err_quit("str_cli: server terminated prematurely");
            }
            write(fileno(fp), buf, n);
        }
        if(FD_ISSET(fileno(fp), &readset)){
            if((n = read(fileno(fp), buf, MAXLINE)) == 0){//read返回0， 表示从终端读到EOF， zero indicates end of file
                stdineof = 1;//该值置1表示客户端关闭终端的写入
                shutdown(sockfd, SHUT_WR);//发送FIN，关闭写端
                FD_CLR(fileno(fp), &readset);
                continue;//继续调用select， 等待
            }
            writen(sockfd, buf, n);
        }
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