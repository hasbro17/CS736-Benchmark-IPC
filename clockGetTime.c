#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
	uint64_t diffNS;//in nano seconds
	struct timespec start, end;
	int counter=0;

	
	//start stop clock
	clock_gettime(CLOCK_MONOTONIC, &start);
	usleep(1000);
	//while(counter<100)
	//	counter++;
	clock_gettime(CLOCK_MONOTONIC, &end);

	diffNS = /*1e9L * (end.tv_sec - start.tv_sec) +*/ end.tv_nsec - start.tv_nsec;
	printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diffNS);

}
