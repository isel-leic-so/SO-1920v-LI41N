 #define _GNU_SOURCE
 
 #include <stdio.h>
 
 #include <sched.h>
 #include <stdlib.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 #include <unistd.h>
 
 
 #define STACK_SIZE (1024*32)
 
 int num=0;
 
 int clone(int (*fn)(void *), void *child_stack,
                 int flags, void *arg, ...
                 /* pid_t *ptid, void *newtls, pid_t *ctid */ );


int child_func(void *arg) {
	printf("I'm child process with pid=%d\n", getpid());
	printf("child: add 2 to global num...\n");
	num += 2;
	printf("child: num=%d\n", num);
	return 0;
}


int main() {
	int child_id;
	
	child_id = clone(child_func, malloc(STACK_SIZE) + STACK_SIZE, 
				0
				, NULL);
	
	if (child_id== -1) {
		perror("error creating child");
	}
	else {
		 
		
		printf("parent: child created with id %d\n", child_id);	
		//sleep(10);
		if (waitpid(-1, NULL,  0 /*__WCLONE */) == -1)
			perror("error waiting for child termination");
		else
		 
			printf("parent: num =%d\n", num);
	}
	
	return 0;
}
