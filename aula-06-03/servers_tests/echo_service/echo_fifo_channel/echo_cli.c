/*
 * implements an echo client using a FIFO (named pipe) based channel
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include "echo_service.h"
#include "chrono.h"
 

#define NITERS 10000


int create_fifo(const char *name) {
	int fd; // fifo file descriptor
 
	int res = mkfifo(name, 0622 /* read/write permission for owner, write for others */);
	
	if (res == -1 && errno != EEXIST) return -1;
	
	
	// to explain later
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) return 1;
	
	
	// In general opening a fifo for read/write is not supported (or ead or write only)
	// this is specific for Linux and greatly simplifies the FIFO management
	// since thre is no blocking in open and read remains blocked even in the absence of other writers
	if ( (fd = open(name, O_RDWR)) == -1) 
		return -1;
	
	printf("server:creation done!\n");
	return fd;
}


int open_server_fifo(const char *service_name) {
	return open(service_name, O_WRONLY);
}

int open_fifo(const char *name) {
	return open(name, O_RDONLY);
}

int main(int argc, char *argv[]) {
	 
	int sfd = open_server_fifo(ECHO_SERVICE_NAME);
	if (sfd == -1) {
		fprintf(stderr, "error opening server channel\n");
		return 1;
	}
	
	// create client fifo
	
	char client_fifo_name[CHANNEL_NAME_MAX_SIZE];
	snprintf(client_fifo_name, CHANNEL_NAME_MAX_SIZE, "%s%d", ECHO_CLIENT_PREFIX, getpid());
	int cfd = create_fifo(client_fifo_name);
	 
	if (cfd == -1) {
		fprintf(stderr, "error creating client fifo\n");
		close(sfd);
		return 1;
	}
	
	echo_msg_t msg, resp;
	msg.sender = getpid();
 
	printf("start...\n");
	
	chrono_t chron = chrono_start();
	for(int try=1; try <= NITERS; try++) {
		sprintf(msg.msg, "Hello_%d_%d", msg.sender, try);
		write(sfd, &msg, sizeof(echo_msg_t));
		// get response
	
		read(cfd, &resp, sizeof(echo_msg_t));
		//printf("%s\n", resp.msg);
	}
	printf("%d tries in %ld micros!\n", NITERS, chrono_micros(chron));
 
	
	close(sfd);
	close(cfd);
	unlink(client_fifo_name);
	 
	printf("client done!\n\n");
	return 0;
}
