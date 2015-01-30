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

int main(int argc, char* argv[])
{
	/*
	//Experiment variables
	int sizes[]={4, 16, 64, 256, 512, 1*K, 4*K, 16*K, 64*K, 256*K, 512*K};
	char* msg;

	//number of times experiment runs
	for(int i=0; i<sizeof(sizes)/sizeof(sizes[0]); i++)
	{
	}
*/

	//Timer variables
	uint64_t diffNS;//in nano seconds
	struct timespec start, end;

	//rdtsc	
	unsigned long long a,b;
	//Get frequency of processor
	a=rdtsc();
	sleep(2);
	b=rdtsc();
	unsigned long long rdtsFreq= (b-a)/2;


	int sendPipe[2], ackPipe[2];
	int childpid;
	char msg[]="Hello\0";
	char readBuff[80];

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

		//Child revieves message from parent
		read(sendPipe[READ], readBuff, sizeof(readBuff));
		//Child sends reply to parent
		write(ackPipe[WRITE], msg, strlen(msg)+1);

		//printf("Child recieved parent msg: %s\n",readBuff);

	}
	else
	{
		//Parent closes unnecessary ends
		close(sendPipe[READ]);
		close(ackPipe[WRITE]);

		//start timer
		//clock_gettime(CLOCK_MONOTONIC, &start);
		a=rdtsc();	

		//Parent sends message to child
		write(sendPipe[WRITE], msg, strlen(msg)+1);
		//Parent recieves message from child
		read(ackPipe[READ], readBuff, sizeof(readBuff));


		//stop timer
		//clock_gettime(CLOCK_MONOTONIC, &end);
		b=rdtsc();
	
	
		//calculate difference	
		//diffNS = 1e9L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
		double diffNS= (double) (1e9*( 1.0*(b-a)/rdtsFreq) );

		printf("Parent recieved reply: %s\n",readBuff);	
		printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diffNS);

	}

	return(0);

}
