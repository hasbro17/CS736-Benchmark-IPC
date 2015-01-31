#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

sem_t empty, full;
int buffSize;
int fill=0;
int use=0;
char* buffer;

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
	printf("Consumer:%d\n",getpid());
	int i;
	for(i=0; i<msgSize; i++)
	{
		sem_wait(&full);
		value=get();
		printf("%s",value);
		sem_post(&empty);
	}

}


void producer(int msgSize)
{
	int i;
	for(i=0; i<msgSize; i++)
	{
		sem_wait(&empty);
		put('0');
		sem_post(&full);
	}

}



int main(int argc, char* arv[])
{
	if(argc!=4)
	{
		perror("Usage:./shareMem [runs] [msg-size] [bufferSize]\n");
		exit(1);
	}

	int fd,pid;
	int runs=atoi(argv[1]);
	int size=atoi(argv[2]);
	buffSize=atoi(argv[3]);//size of buffer

	sem_init(&empty, 1, buffSize);
	sem_init(&full, 1, 0);


	fd=open("sharedFile", O_RDWR | O_CREAT, S_IRWXU);

	buffer=mmap(NULL, buffSize, PROT_READ | PROT_WRITE |, MAP_SHARED, fd, 0);

	close(fd);

	if( (pid=fork())<0 )
	{
		perror("Fork error\n");
		exit(1);
	}
	
	if(pid==0)
	{
		consumer(size);

	}
	else
	{
		producer(size);
	}


	return 0;
}
