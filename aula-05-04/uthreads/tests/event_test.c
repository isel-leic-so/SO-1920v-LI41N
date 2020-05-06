#include <unistd.h>
#include <stdio.h>

#include "../include/uthread.h"
#include "../include/usynch.h"

void notifier(UT_ARGUMENT arg) {
	PEVENT done  = (PEVENT)arg;
	printf("notifier thread begin\n");
	usleep(3000000);
	event_set(done);
	printf("notifier thread end\n");
}


void waiter(UT_ARGUMENT arg) {
	PEVENT done = (PEVENT)arg;
	printf("waiter  begin\n");
	printf("waiter: wait for thread notification!\n");
	event_wait(done);
	printf("waiter thread end!\n");
	
}

void event_test() {
	EVENT done;
	
	printf("event test  begin\n");
	
	event_init(&done);
	
	ut_create(notifier, &done);
	ut_create(waiter, &done);
	
	ut_run();
	
	printf("event test  end\n");
}


int main() {
	ut_init();
	event_test();
	ut_end();
    return 0;
}

