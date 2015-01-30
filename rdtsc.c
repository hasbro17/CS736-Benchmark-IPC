#include <stdio.h>
#include "rdtsc.h"

int main(int argc, char* argv[])
{

	unsigned long long a,b;
	double result;
	int counter=0;

	//Get frequency of processor
	a=rdtsc();
	sleep(2);
	b=rdtsc();
	unsigned long long rdtsFreq= (b-a)/2;

	//Measure precision using rdtsc
	a=rdtsc();
	usleep(1000);
	//while(counter<100)
	//	counter++;
	b=rdtsc();
	double rdtscNS= (double) (1e9*( 1.0*(b-a)/rdtsFreq) );

	//Measure precision using 
	printf("clock cycles %llu\n", b-a);
	printf("time elapsed in nanosecs %g\n",rdtscNS);
	return 0;
}

