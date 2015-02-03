#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <stdint.h>



int buffSize;
int fill=0;
int use=0;
char* buffer;

typedef struct {
	sem_t empty;
	sem_t full;
	sem_t parent;
	sem_t child;
}SemPair;

SemPair* sem;


void put(char value)
{
	buffer[fill]=value;
	fill=(fill+1)%buffSize;
}

char get()
{
	char tmp=buffer[use];
	use=(use+1)%buffSize;
	return tmp;
}


void consumer(int msgSize)
{
	char value;
	//int pid=getpid();
	//printf("Consumer:%d\n",pid);
	int i;
	for(i=0; i<msgSize; i++)
	{
		sem_wait(&(sem->full));
		value=get();
		//printf("Consumer got %c\n",value);
		sem_post(&(sem->empty));
	}

}


void producer(int msgSize)
{
	//int pid=getpid();
	//printf("Producer:%d\n",pid);
	int i;
	//char value='0';
	for(i=0; i<msgSize; i++)
	{
		sem_wait(&(sem->empty));
		put('0');
		//printf("Producer put %c\n",value);
		//value=(char) value+1;
		sem_post(&(sem->full));
	}

}



int main(int argc, char* argv[])
{
	if(argc!=4)
	{
		perror("Usage:./shareMem [total-size-MB] [packet-size-bytes] [bufferSize-bytes]\n");
		exit(1);
	}


	int totalSizeMB=atoi(argv[1]);
	uint64_t totalSize=1024LL*1024LL*((uint64_t)(totalSizeMB));
	int size=atoi(argv[2]);
	char* msg= (char*) malloc(size);
	memset(msg, '0', size);
	char readBuff[size];
	char ack='a';
	long double throughput;
	
	uint64_t runs= totalSize/(size*1LL);
	long tempruns=runs;


	//Timer variables
	long double diffNS;//in nano seconds
	struct timespec start, end;

	int fd,pid;
	buffSize=atoi(argv[3]);//size of buffer

	//share semaphore
	int shm = shm_open("sharedSem", O_RDWR | O_CREAT, S_IRWXU);
	ftruncate(shm, sizeof(SemPair));
	sem=mmap(NULL, sizeof(SemPair), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);


	sem_init(&(sem->empty), 1, buffSize);
	sem_init(&(sem->full), 1, 0);



	fd=open("sharedFile", O_RDWR | O_CREAT, S_IRWXU);
	ftruncate(fd, buffSize);
	buffer=mmap(NULL, buffSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	close(fd);

	sem_init(&(sem->parent), 1, 0);
	sem_init(&(sem->child), 1, 0);

	if( (pid=fork())<0 )
	{
		perror("Fork error\n");
		exit(1);
	}
	
	if(pid==0)//child
	{
		while(runs>0)
		{
			consumer(size);
			runs--;
		}
			sem_post(&(sem->parent));
			producer(sizeof(ack));

	}
	else//parent
	{
		
			
		//start timer
		clock_gettime(CLOCK_MONOTONIC, &start);
		
		while(runs>0)
		{
			producer(size);
			runs--;
		}
		
		//wait for child to become producer
		sem_wait(&(sem->parent));
		
		//recieve ack
		consumer(sizeof(ack));
		
		//stop timer
		clock_gettime(CLOCK_MONOTONIC, &end);

		//calculate throughput	
		diffNS = (end.tv_sec - start.tv_sec) + 1e-9L*(end.tv_nsec - start.tv_nsec);
		printf("Time taken:%Lfs and packet size:%llu bytes\n", diffNS,(long long unsigned int) totalSize);	
		throughput=((long double)totalSizeMB*1.0) / (diffNS);
		printf("Throughput for %lu packets: %Lf Mbps\n", tempruns, throughput);
	}


	return 0;
}
