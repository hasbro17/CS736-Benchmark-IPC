#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_uint64 0xFFFFFFFFFFFFFFFF

int main(int argc, char *argv[])
{
	int sockfd = 0, count = 0, rounds = 0, size=0, delay=1;
	char *buf,*ack;
	struct sockaddr_in serv_addr; 
	
	uint64_t diffNS,minNS;//in nano seconds
	minNS=MAX_uint64;
	struct timespec start, end;
	
	if(argc != 5){
		printf("\n Usage: %s <ip of server> <port number> <rounds> <size in mb>\n",argv[0]);
		return 1;
	} 
	
	rounds = atoi(argv[3]);
	size = atoi(argv[4])*1024*1024;
	
	buf = (char*)malloc(sizeof(char)*size);
	ack = (char*)malloc(sizeof(char)*4);
	
	memset(buf,'0',sizeof(char)*size);
	memset(ack,'0',sizeof(char)*4);
	
	printf("\nRound = %d",rounds);
	printf("\nSize = %d",size);
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Error : Could not create socket \n");
		return 1;
	}
		
	//setsockopt(sockfd,SOL_TCP,TCP_NODELAY,&delay,sizeof(delay));
	
	memset(&serv_addr, '0', sizeof(serv_addr)); 
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2])); 
	
	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0){
		printf("\n inet_pton error occured\n");
		return 1;
	} 
	
	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("\n Error : Connect Failed \n");
		return 1;
	} 
		
	while(rounds--){
		
		clock_gettime(CLOCK_MONOTONIC, &start);
		
		//write the data to the server
		printf("\nWriting the data for size = %d",size);
		write(sockfd, buf, size);
		
		//get the ack
		read(sockfd, ack, 4);
		printf("\nRead the data");
		
		clock_gettime(CLOCK_MONOTONIC, &end);
		
		diffNS = 1e9L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
		
		
		if(diffNS<minNS)
			minNS = diffNS;
	}
	
	printf("\nMin elapsed time = %llu nanoseconds\n", (long long unsigned int) diffNS);

	//close(sockfd);
	
	return 0;
}
