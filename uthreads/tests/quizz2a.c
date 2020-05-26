#include <stdio.h>

#include "../include/uthread.h"

enum state { Ready, Running, Blocked };

int ut_get_state(HANDLE) {
}

state_as_string() {
	enum state st = ut_get_state(ut_self());
	
	if (st == Running) return "Running";
	else if (st == Ready) return "Ready";
	else if(st == Blocked) return "Blocked";
	else return "Unknown";
}

long F1(UT_ARGUMENT arg) {
	long status;
	printf("Thread %ld %s\n", (long)arg, state_as_string());
	UtYield();
	printf("Thread %ld %s\n", (long)arg, state_as_string());
	return 0;
} 

long F2(UT_ARGUMENT arg) {
	printf("Thread %ld %s\n", (long)arg, ut_state_as_string());
	UtYield();
	printf("Thread %ld %s\n", (long)arg, ut_state_as_string());
	return 0;
} 


int main() {
	UtInit();
	UtCreate(F1, (UT_ARGUMENT) 1L );
	UtCreate(F2, (UT_ARGUMENT) 2L);
	
 
	UtRun();
	 
	UtEnd();
}
