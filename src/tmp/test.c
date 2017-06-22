#include "unp.h"

int main(){
    
    char buf[MAXLINE];
    while(1){
        if(fgets(buf, MAXLINE, stdout) != NULL)
            printf("%s", buf);
    }
    return 0;

    struct timeval time;
    time.tv_usec
    
}