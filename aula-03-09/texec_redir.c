#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <fcntl.h>



int my_system(const char* prog, const char *outfile) {
	int child;
	int status;
	int fd_out;
	
	
	
	if ((child = fork()) == 0) {
		if ((fd_out= open(outfile, O_WRONLY | O_CREAT, 0777)) == -1) {
			fprintf(stderr, "error %d creating outfile\n", errno);
			return -1;
		}
		
		close(1); // close standard output
		
		dup(fd_out);
		close(fd_out);
		
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
	if (argc != 3) {
		printf("usage: texec <cmd> <outfile>!\n");
		return 1;
	}
	
	my_system(argv[1], argv[2]);
	return 0;
		
}

