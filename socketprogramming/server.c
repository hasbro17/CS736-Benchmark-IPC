#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

int main(int argc, char *argv[])
{
    if(argc != 4){
	printf("\nUsage: ./server <port_number> <rounds> <size of buffer in bytes>");
	return 0;
    }

    int listenfd = 0, connfd = 0, count = 0, delay =1;
    int rounds = atoi(argv[2]);
    int size = atoi(argv[3]);

    struct sockaddr_in serv_addr; 

    char *buf = (char*)malloc(sizeof(char)*size);
		
		printf("\nPort number = %s",argv[1]);
    printf("\nNumber of rounds = %d ",rounds);
    printf("\nSize = %d ",size);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
		//setsockopt(listenfd,SOL_TCP,TCP_NODELAY,&delay,sizeof(delay));
		
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(buf,'0',sizeof(char)*size);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1])); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 1); 

    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

    while(rounds--){

    read(connfd,buf,size);

    //printf("\nReceived: %d",count++);
    //snprintf(buf,1024,"%d",count+2);

    write(connfd, buf, size);
    

    }

    close(connfd);

    return 0;
}
