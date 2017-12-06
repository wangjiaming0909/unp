#include "unp.h"
#include <limits.h>
#include "headers.h"

void sig_chld(int);

void str_echo(int sockfd){
    err_msg("calling str_echo..: %s", strerror(errno));
    ssize_t n;
    char    buf[MAXLINE];
again:
    while((n = readline(sockfd, buf, MAXLINE))> 0)
        writen(sockfd, buf, n);
    if(n < 0 && errno == EINTR)//被信号中断
        goto again;
    else if(n < 0)
        err_sys("str_echo: read error");
}


/*
    改进版服务器程序可以解决一下三个问题
    1， 当fork子进程时，捕获SIGCHLD信号
    2， 捕获信号好后，处理被中断的系统调用， 父进程正阻塞在accept调用时，接收到SIGCHLD信号，
        accept被中断，因此在处理SIGCHLD信号时，需要给sigaction函数提供SA_RESART标志：act.sa_flags |= SA_RESTART
    3， 使用waitpid避免产生僵尸进程
*/
int maintcpserv(int argc, char *argv[]){
    int         listenfd, connfd, ret;
    pid_t       child_pid;
    socklen_t   clilen;
    struct sockaddr_in servaddr, cliaddr;

    bzero(&servaddr, sizeof(servaddr));
    bzero(&cliaddr, sizeof(cliaddr));

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        err_msg("socket error:%s", strerror(errno));
        exit(-1);
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(1001);

    if((ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0){
        err_msg("bind error:%s", strerror(errno));
        exit(-1);
    }
    if((ret = listen(listenfd, LISTENQ)) < 0){
        err_msg("listen error:%s", strerror(errno));
        exit(-1);
    }
    for(;;){
        clilen = sizeof(cliaddr);
        if((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0){
            err_msg("accept error:%s", strerror(errno));
            exit(-1);
        }
        signal(SIGCHLD, sig_chld);
        if((child_pid = fork()) == 0){
            close(listenfd);
            //err_msg("child process called..");
            str_echo(connfd);
            close(connfd);
            exit(0);
        }
        close(connfd);
    }
    return 0;
}


/*
    改进服务端程序
    1, 单进程接受连接请求，不调用fork，不需要捕捉SIGCHLD信号
    2, client[FD_SETSIZE], 初始化为-1， 使用其第一个可用项记录新到达的连接的描述符
    3, select
*/
int maintcpserv3(int argc, char** argv){
    int                 i, maxi, maxfd, listenfd,connfd, sockfd;
    int                 nready, client[FD_SETSIZE];
    ssize_t             n, ret;
    fd_set              rset, allset;
    char                buf[MAXLINE];
    socklen_t           clilen;
    struct sockaddr_in  cliaddr, servaddr;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_quit("socket error: %s", strerror(errno));
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1001);

    if((ret = bind(listenfd, (SA*)&servaddr, sizeof(servaddr))) < 0)
        err_quit("bind error: %s", strerror(errno));
    if((ret = listen(listenfd, LISTENQ)) < 0)
        err_quit("listen error: %s", strerror(errno));
    
    maxfd = listenfd;
    FD_SET(listenfd, &rset);

    //初始化client数组
    for(i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;
    
    maxi = -1;//max index in client[] array
    FD_ZERO(&allset);//allset用于记录所有服务器需要select的描述符
    FD_SET(listenfd, &allset);

    for(;;){
        rset = allset;
        if((nready = select(maxfd + 1, &rset, NULL, NULL, NULL)) <= 0)
            err_quit("select error:%s", strerror(errno));
        
        if(FD_ISSET(listenfd, &rset)){//listenfd套接字可读
            clilen = sizeof(cliaddr);
            if((connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) < 0)
                err_quit("accept error: %s", strerror(errno));
            
            //处理新到的描述符：
            for(i = 0; i < FD_SETSIZE; i++){
                if(client[i] < 0){//client数组中第一个可用项记录新到达的描述符
                    client[i] = connfd;
                    break;
                }  
            }
            if(i == FD_SETSIZE)
                err_quit("too many clients");
            //新到达一个描述符，需要加入allset中
            FD_SET(connfd, &allset);
            //调整maxfd
            maxfd = max(connfd, maxfd);
            if(i > maxi)
                maxi = i;
            
            if(--nready <= 0)//no more readable descriptors
                continue;
        }

        for(i = 0; i <= maxi; i++){//check all clients for data
            if((sockfd = client[i]) < 0)
                continue;
            if(FD_ISSET(sockfd, &rset)){
                if((n = read(sockfd, buf, MAXLINE)) == 0){
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }else  
                    writen(sockfd, buf, n);
                if(--nready <= 0)
                    break;
            }
        }
        
    }


}

/*
    改进版：
    1，使用poll函数


*/
int mainasasd(int argc, char **argv){
    int                 listenfd, connfd, sockfd, i, n, maxi;
    int                 nready;
    char                buf[MAXLINE];
    socklen_t           clilen;
    struct sockaddr_in  cliaddr, servaddr;
    struct pollfd       client[_SC_OPEN_MAX];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1001);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    //初始化pollfd结构
    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for(i = 1; i < _SC_OPEN_MAX; i++)
        client[i].fd = -1;
    maxi = 0;//max index

    for(; ;){
        nready = poll(client, maxi + 1, INFTIM);
        if(client[0].revents & POLLRDNORM){
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (SA*)&cliaddr, &clilen);

            for(i = 1; i < _SC_OPEN_MAX; i++){//找到第一个是-1的fd，将新连接的描述符填进去
                if(client[i].fd < 0){
                    client[i].fd = connfd;
                    break;//找到之后就退出循环
                }
            }
            if(i == _SC_OPEN_MAX)
                err_quit("too many clients");
            client[i].events = POLLRDNORM;

            if(i > maxi)
                maxi = i;
            if(nready-- <= 0)
                continue;
        }

        for(i = 1; i <= maxi; i++){//check all clients for data
            if((sockfd = client[i].fd) < 0)
                continue;
            if(client[i].revents & (POLLRDNORM | POLLERR)){
                if((n = read(sockfd, buf, MAXLINE)) < 0){
                    if(errno == ECONNRESET){
                        close(sockfd);
                        client[i].fd = -1;
                    }else
                        err_quit("read error");
                }else if(n == 0){
                    close(sockfd);
                    client[i].fd = -1;
                }else
                    writen(sockfd, buf, n);

                if(--nready <= 0)
                    break;                          //no more readable descriptors
            }
        }
    }

}

/*
    当终止子进程时：
        子进程阻塞于read函数，
        此时kill子进程，关闭所有打开的描述符，向客户端发送一个FIN，进入FIN_WAIT1
        客户端回复一个ACK，客户端进入CLOSE_WAIT， 服务端进入FIN_WAIT2；
        父进程接收到SIGCHLD信号，回收子进程；
        客户端继续向该子进程发送数据，调用write函数，服务端接收到数据，由于之前的套接字已经终止，返回一个RST
        由于子进程发送了FIN，因此客户端认为服务端不会再给自己发送数据，因此read直接返回0， 表示EOF，

*/
