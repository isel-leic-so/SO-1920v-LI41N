#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>



int main() {
	// create pipe
	
	int p[2];
	
	if (pipe(p) != 0) {
		fprintf(stderr, "error %d creating pipe!\n", errno);
		return 1;
	}
	// create child process
	
	if (fork() == 0) {
		// child!
		const char *msg = "hello from child!";
		
		// close the pipe read side"
		close(p[0]);
		write(p[1] /*write side */, msg, strlen(msg) + 1);
		exit(0);
	}
	else {
		// parent !
		
		// free the pipe write side!
		close(p[1]);
		char resp[100]; // maximum response size
		int idx = 0; // read position
		int status;
		
		while (read(p[0], &resp[idx], 1) == 1)  idx++;
		
		printf("message from child: %s\n", resp);
		
		// wait for child termination
		wait(&status);
		printf("done!\n");
	}
	return 0;
}
