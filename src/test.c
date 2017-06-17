#include "unp.h"
#include <stdarg.h>

int readn_readline(){
    int fd;
    ssize_t n;
    char *buf = (char *)malloc(64);
    bzero((void *)buf, 64);

    fd = open("/home/www/Documents/1.c", O_RDONLY);
    // if((n = readn(fd, (void *)buf,64)) < 0){
    //     err_quit("error readn: %s\n", strerror(errno));
    // }
    // printf("%s\n", buf);
    if((n = readline(fd, (void *)buf, 100)) < 0){
        err_quit("error readline: %s\n", strerror(errno));
    }
    printf("%s\n", buf);
    if(buf != NULL)
        free(buf);
    return 0;
}

int test_addr_cast_func(){
    const char      *paddr = "0x123";//0x7, 0.0.122.83
    int             addr_int, ret;
    struct in_addr  addr;
   
#if 0
    //inet_aton
    ret = inet_aton(paddr, &addr);
    if(ret != 1){
        err_quit("inet_pton error:%s\n", strerror(errno));
    }
    addr_int = (int)addr.s_addr;
    printf("ipv4地址对应的int为:%d\n", addr_int);//209419131
    return 0;
#endif//inet_aton

#if 0
    // inet_ntoa 
   struct in_addr addr_struct;
   addr_struct.s_addr = 1400504320;
   char *addr_buf = (char*)malloc(32);
   addr_buf = inet_ntoa(addr_struct);
   printf("%s\n", addr_buf);//123.123.123.12
   return 0;
#endif //inet_ntoa

#if 0
    //inet_pton
    unsigned char addrv6[16];
    ret = inet_pton(AF_INET6, "2001:da8:7007:412::19", addrv6);
    if (ret == 0){
        printf("not the valid ipaddress\n");
        return -1;
    }else if(ret == -1)
        err_quit("inet_pton error %s\n", strerror(errno));
    else
        printf("ip_int:%d\n", addrv6);
    //return 0;
#endif //inet_pton

#if 0
    //inet_ntop
    // unsigned char addrptr[16] = ?? ;
    char buf[INET_ADDRSTRLEN];
    paddr = inet_ntop(AF_INET6, addrv6, buf, INET6_ADDRSTRLEN);
    if(paddr == NULL)
        err_quit("inet_ntop error %s\n", strerror(errno));
    printf("%s\n", buf);
    return 0;
#endif //inet_ntop

#if 0
    //inet_pton_loose ipv4
    ret = inet_pton_loose(AF_INET, paddr, &addr_int);
    printf("%d\n", addr_int);
        return 0;
#endif

#if 1
    //ient_pton_loose ipv6
    char buf[INET_ADDRSTRLEN];
    ret = inet_pton_loose(AF_INET6, "123.123.1.1", buf);
    
#endif

}



int main(){
    // readn_readline();
    test_addr_cast_func();
    return 0;

}
