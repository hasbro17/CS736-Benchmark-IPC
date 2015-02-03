#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "rdtsc.h"

#define K 1024
#define READ 0
#define WRITE 1
#define MAX_uint64 0xFFFFFFFFFFFFFFFF

int main(int argc, char* argv[])
{

	if(argc!=3)
	{
		perror("Usage:./pipe [total-msg-size] [packet-size]\n");
		exit(1);
	}

	long totalSize=atol(argv[1]);
	int size=atoi(argv[2]);
	char* msg= (char*) malloc(size);
	memset(msg, '0', size);
	char readBuff[size];
	char ack='a';
	
	int runs= totalSize/size;
	int tempruns=runs;

	//Timer variables
	uint64_t diffNS,minNS;//in nano seconds
	minNS=MAX_uint64;
	struct timespec start, end;

/*
	//rdtsc	
	unsigned long long a,b;
	//Get frequency of processor
	a=rdtsc();
	sleep(2);
	b=rdtsc();
	unsigned long long rdtsFreq= (b-a)/2;
*/

	int sendPipe[2], ackPipe[2];
	int childpid;


	if( pipe(sendPipe)!=0 || pipe(ackPipe)!=0)
	{
		perror("pipe error");
		exit(1);
	}
	
	if( (childpid = fork()) == -1)
	{
		perror("fork error");
		exit(1);
	}

	if(childpid == 0)
	{
		//Child closes unnecessary ends
		close(sendPipe[WRITE]);
		close(ackPipe[READ]);

		while(runs>0)
		{
			//Child revieves message from parent
			read(sendPipe[READ], readBuff, size);
			
			//printf("Child recieved parent msg: %s\n",readBuff);
			runs--;
		}

		//Child sends reply to parent
		write(ackPipe[WRITE], ack, sizeof(ack));

	}
	else
	{
		//Parent closes unnecessary ends
		close(sendPipe[READ]);
		close(ackPipe[WRITE]);

		//start timer
		clock_gettime(CLOCK_MONOTONIC, &start);
		//a=rdtsc();	

		while(runs>0)
		{
			//Parent sends message to child
			write(sendPipe[WRITE], msg, size);
			runs--;	
		}
		//Parent recieves ack from child
		read(ackPipe[READ], readBuff, size);

		//stop timer
		clock_gettime(CLOCK_MONOTONIC, &end);
		//b=rdtsc();
	
	
		//calculate difference	
		diffNS = 1e9L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
			//double diffNS= (double) (1e9*( 1.0*(b-a)/rdtsFreq) );

			//printf("Parent recieved reply: %s\n",readBuff);	
			//printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diffNS);
		printf("Throughput for %d packets: %llu nanoseconds\n", tempruns, (long long unsigned int) minNS/2);

	}

	return(0);

}
