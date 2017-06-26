#include "unp.h"
#include "headers.h"

int
main04(){
    union{
        short   s;
        char    c[sizeof(short)];
    }un;

    un.s = 0x0102;
#ifdef CPU_VENDOR_OS
    printf("%s: ", CPU_VENDOR_OS);
#else
    printf("no CPU_VENDOR_OS\n");
#endif
    if(sizeof(short) == 2){
        if(un.c[0] == 1 && un.c[1] == 2)
            printf("big-endian\n");
        else if(un.c[0] == 2 && un.c[1] == 1)
            printf("little-endian\n");
        else
            printf("unknow\n");
    }else
        printf("sizeof(short) = %ld\n", sizeof(short));
    exit(0);
}
