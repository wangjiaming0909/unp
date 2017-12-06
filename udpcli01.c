#include "unp.h"
#include "headers.h"

void dg_cli(FILE *fp, int sockfd, SA *pservaddr, socklen_t servlen){
	int n;
	char sendline[MAXLINE], recvline[MAXLINE+1];
	while(fgets(sendline, MAXLINE, fp) != NULL){
		if(sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen) < 0)
			err_quit("error sendto");
		if((n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL)) < 0)
			err_quit("error recvfrom");
		recvline[n] = 0;
		if(fputs(recvline, stdout)  == EOF)
			err_quit("error fputs");
	}
}

int mainududpcli(int argc, char **argv){

	int sockfd, ret;
	struct sockaddr_in servaddr;
	argc = 2;
	argv[1] = "139.224.239.147";
	if(argc != 2)
		err_quit("usage: unp <IPaddress>");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1001);
	if((ret = inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr)) != 1){
		if(ret == 0)
			err_quit("invalid ipaddress");
		else if(ret == -1)
			err_quit("error inet_pton");
	}

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		err_quit("error socket");
	dg_cli(stdin, sockfd, (SA*) &servaddr, sizeof(servaddr));
	exit(0);
}
