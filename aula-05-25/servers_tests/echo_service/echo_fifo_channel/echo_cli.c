/*
 * implements an echo server using a ring buffer based memory channel
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "echo_service.h"
 

#define NTRIES 10000

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
	
	int res = mkfifo(client_fifo_name, 0622 /* read permission for owner, write for others */);
	
	if (res == -1 && errno != EEXIST) {
		fprintf(stderr, "error creating client fifo\n");
		close(sfd);
		return 1;
	}
	

		
	echo_msg_t msg, resp;
	msg.sender = getpid();
 
	printf("start...\n");
	
	for(int try=1; try <= NTRIES; try++) {
		sprintf(msg.msg, "Hello_%d_%d", msg.sender, try);
		write(sfd, &msg, sizeof(echo_msg_t));
		// get response
		

		int cfd = open_fifo(client_fifo_name);
		if (cfd == -1) {
			fprintf(stderr, "error opening client fifo\n");
			close(sfd);
			return 1;
		}
	
		read(cfd, &resp, sizeof(echo_msg_t));
		close(cfd);
		printf("%s\n", resp.msg);
	}

 
	unlink(client_fifo_name);
	close(sfd);
	 
	printf("client done!\n\n");
	return 0;
}
