#include "unp.h"
#include "headers.h"

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen){
	int 				n;
	socklen_t		len;
	char 			mesg[MAXLINE];

	for(;;){
		len = clilen;
		if((n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len)) < 0)
			err_quit("recvfrom error");
		if((n = sendto(sockfd, mesg, MAXLINE, 0, pcliaddr, len)) < 0)
			err_quit("sendto error");
	}
}

int main(int argc, char **argv){
	int 							sockfd;
	struct sockaddr_in 	servaddr, cliaddr;

	bzero(&servaddr, sizeof(servaddr));
	bzero(&cliaddr, sizeof(cliaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1001);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		err_quit("socket error");
	if(bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0)
		err_quit("bind error");
	dg_echo(sockfd, (SA*)&cliaddr, sizeof(cliaddr));
}
