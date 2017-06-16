/*************************************************************************
	> File Name: sock_func.c
	> Author: www
	> Mail: 
	> Created Time: Fri 16 Jun 2017 11:09:31 AM CST
 ************************************************************************/
//协议无关的函数
#include "unp.h"

char *sock_ntop(const struct sockaddr* sa, socklen_t salen);
int sock_bind_wild(int sockfd, int family);
int sock_cmp_addr(const struct sockaddr *sockaddr1,
                 const struct sockaddr *sockaddr2, socklen_t addrlen);
int sock_get_port(const struct sockaddr *sockaddr, socklen_t addrlen);
char *sock_ntop_host(const struct sockaddr *sockaddr, socklen_t addrlen, void *ptr);
void sock_set_port(const struct sockaddr *sockaddr, socklen_t addrlen, int port);
void sock_set_wild(struct sockaddr *sockaddr, socklen_t addrlen);

char sock_ntop(const struct sockaddr *sa, socklen_t salen){
    char portstr[8];
    static char str[128];

    switch(sa->sa_family){
        case AF_INET:{
            struct sockaddr_in *sin = (struct sockaddr_in *)sa;
            if(inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
                return NULL;
            if(ntohs(sin->sin_port) != 0){
                snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
                strcat(str, portstr);
            }
            return str;
        }
    
    }
}
