#include <stdio.h>

int main(){
    
	typedef union{
		char c[13];
		int i;
	}u1;

	printf("sizeof(u1) : %ld", sizeof(u1));



//    char buf[MAXLINE];
//    while(1){
//        if(fgets(buf, MAXLINE, stdout) != NULL)
//            printf("%s", buf);
//    }
//    return 0;

//    struct timeval time;
//    time.tv_usec
    
}
