/*
 * implements an unix socket echo server 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include "echo_service.h"

 
// size of pending connections queue
#define BACKLOG 128


int create_bind_socket(const char* sock_name) {
	int sfd;
	struct sockaddr_un srv_addr;
	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd == -1) return -1;
	
	// remove old entry if exsists
	if (unlink(sock_name) == -1 && errno != ENOENT) {
		close(sfd);
		return -2;
	}
		 
		
	/*  bind socket */
	memset(&srv_addr, 0, sizeof(struct sockaddr_un));
	srv_addr.sun_family = AF_UNIX;
	strncpy(srv_addr.sun_path, sock_name, sizeof(srv_addr.sun_path) - 1);
	
	if (bind(sfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_un)) == -1) {
		close(sfd);
		return -3;
	}
	
	return sfd;
}

void process_connection(int cfd) {
	echo_msg_t msg;
	int nread;
	
	while ((nread = read(cfd, &msg, sizeof(echo_msg_t))) == sizeof(echo_msg_t)) {
		write(cfd, &msg, sizeof(echo_msg_t));
	}
	close(cfd);	
		 
}


void run(int sfd) {
	int cfd; // connection socket
	for (;;) {  
		 
		cfd = accept(sfd, NULL, NULL);
		if (cfd == -1) {
			fprintf(stderr, "error creating socket\n");
			return;
		}
			
		process_connection(cfd);	 
	}
}

int main(int argc, char *argv[]) {
	int sfd;
	
	// create server socket
	if ((sfd = create_bind_socket(ECHO_SERVICE_NAME)) < 0) {
		fprintf(stderr, "error creating socket\n");
		return 1;
	}
 
    // set listen queue size
	if (listen(sfd, BACKLOG) == -1) {
		fprintf(stderr, "error setting  listen queue size\n");
		return 1;
	}
	
	// run the server
	run(sfd);
	
	unlink(ECHO_SERVICE_NAME);
	close(sfd);
}


 
