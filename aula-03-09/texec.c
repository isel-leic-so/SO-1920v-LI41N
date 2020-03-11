#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>



int my_system(const char* prog) {
	int child;
	int status;
	
	if ((child = fork()) == 0) {
		// child process
		execl(prog, prog, NULL);
		fprintf(stderr, "error %d doing exec\n", errno);
		exit(1);
	}
	else if (child <0) {
		fprintf(stderr, "error %d creating child process!\n", errno);
		return -1;
	}
	else {
		// parent code, waiting for child
		return waitpid(child, &status, 0);
	}
	
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: texec <cmd>!\n");
		return 1;
	}
	
	my_system(argv[1]);
	return 0;
		
}

