#include <stdio.h>

#include "../include/uthread.h"

HANDLE t1, t2, t3, t4;

long f1(UT_ARGUMENT arg) {
	long status;
	printf("Thread %ld start\n", (long)arg);
	UtGetThreadResult(t2, &status);
	UtYield();
	printf("Thread %ld end\n", (long)arg);
	return 0;
} 

long f2(UT_ARGUMENT arg) {
	printf("Thread %ld start\n", (long)arg);
	UtYield();
	printf("Thread %ld end\n", (long)arg);
	return 0;
} 

long f3(UT_ARGUMENT arg) {
	LONG status;
	printf("Thread %ld start\n", (long)arg);
	UtGetThreadResult(t2, &status);
	printf("Thread %ld end\n", (long)arg);
	return 0;
}

long f4(UT_ARGUMENT arg) {
	printf("Thread %ld start\n", (long)arg);
	UtYield();
	printf("Thread %ld end\n", (long)arg);
	return 0;
} 

int main() {
	UtInit();
	t1 = UtCreate(f1, (UT_ARGUMENT) 1L );
	t2 = UtCreate(f2, (UT_ARGUMENT) 2L);
	t3 = UtCreate(f3, (UT_ARGUMENT) 3L);
	t4 = UtCreate(f4, (UT_ARGUMENT) 4L);
 
	UtRun();
	 
	UtEnd();
}
