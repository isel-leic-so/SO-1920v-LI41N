#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <unistd.h>
#include <time.h>

#define NCALLS 1000000

/// Returns the number of micros since an undefined time (usually system startup).
static long get_micros()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_nsec / 1000) + (ts.tv_sec * 1000000ul);
}

/// Computes the elapsed time, in milliseconds, between two 'timespec'.
inline int TimeElapsedMs(const struct timespec* tStartTime, const struct timespec* tEndTime)
{
   return 1000*(tEndTime->tv_sec - tStartTime->tv_sec) +
          (tEndTime->tv_nsec - tStartTime->tv_nsec)/1000000;
}


int getval() {
	for(int i=0; i < 10; ++i);
	return 3;
}

int main() {
	long start = get_micros();
	int nice_val;
	
	for(int i=0; i < NCALLS; ++i) {
		nice_val = getpriority(PRIO_PROCESS, getpid());
		if (i==0) printf("priority=%d\n", nice_val);
	}
	
	
	printf("%d syscalls in %ld micros\n", NCALLS, get_micros()-start);
	printf("nice value=%d\n", nice_val);
	
	start = get_micros();
	 	
	for(int i=0; i < NCALLS; ++i) 
		nice_val = getval();
	
	
	printf("%d calls in %ld micros\n", NCALLS, get_micros()-start);
	printf("value=%d\n", nice_val);
	return 0;
}
