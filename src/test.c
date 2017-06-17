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
    const char      *paddr = "123.123.123.12";
    int             addr_int, ret;
    struct in_addr  addr;
    //inet_aton
    ret = inet_aton(paddr, &addr);
    if(ret != 1){
        err_quit("inet_pton error:%s\n", strerror(errno));
    }
    addr_int = (int)addr.s_addr;
    printf("ipv4地址对应的int为:%d\n", addr_int);
    return 0;
   

    
}
//第三章习题3.3
//inet_pton()
//
int inet_pton_loose(){
    int fd;
    return 0;
}
//


int main(){
    // readn_readline();
    test_addr_cast_func();
    return 0;

}
