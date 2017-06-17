#include "unp.h"
#include "headers.h"
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

//
int main(){
    // readn_readline();
    return 0;

}