#include <stdio.h>
#include <unistd.h>	// fork, sleep
#include <errno.h>
#include <stdlib.h>	// exit
#include <sys/wait.h>	// wait
#include <sys/types.h>	//wait 

int main() {
	pid_t parent;
	printf("parent id = %d\n", parent = getpid());
	pid_t child;
	int status;
	
	int i= 1;
	
	if ((child = fork()) == 0) {
		// child process
		printf("I,m child with parent %d(%d)\n", getppid(), parent);
		printf("and process id = %d\n", getpid());
		 
	}
	else if (child > 0) {
		
		printf("child created with id = %d\n", child);
		
	} 
	else {
		printf("error %d creating child!\n", errno);
	};
	
	while (i <= 5) {
		printf("%d: i'm process %d\n", i, getpid());
		++i;
		sleep(1);
	}
	
	wait(&status);
	return 0;
}
