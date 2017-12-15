#ifndef UNP_H
#define UNP_H

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAXLINE 4096
#define LISTENQ 1024

/*----------------封装 socket API----------------*/
void Listen(int fd, int backlog){
    char *ptr;
    if((ptr = getenv("LISTENQ")) != NULL)
        backlog = atoi(ptr);
    
    if(listen(fd, backlog) < 0){
        std::cout << "f listen error.." << std::endl;
        std::cout << strerror(errno) << std::endl;       
        exit(-1);
    }

}
/*----------------封装 socket API----------------*/
#endif // UNP_H
