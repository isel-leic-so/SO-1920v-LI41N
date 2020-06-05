/*
 * implements a concurrent echo server using a stream IP socket and the class threadpool
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "echo_service.h"
#include "tpool.h"
 
// size of pending connections queue
#define BACKLOG 5

// the thread pool
static tpool_t tpool;

int create_bind_server_socket(const char* ip_addr, int port) {
	int sfd;
	struct sockaddr_in srv_addr;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) return -1;
	
 
	/*  bind socket */
	bzero(&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr(ip_addr);
	srv_addr.sin_port = htons(port);
	
	if (bind(sfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1) {
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

void dispatch_connection(void *arg) {
	int cfd = (int)(size_t) arg;
	process_connection(cfd);

}

void run(int sfd) {

	for (;;) {  
		int cfd; // connection socket
		struct sockaddr_in srv_addr;
		socklen_t addrlen = sizeof(struct sockaddr_in);
		cfd = accept(sfd, (struct sockaddr *)  &srv_addr, &addrlen);
		if (cfd == -1) {
			fprintf(stderr, "error creating socket\n");
			return;
		}
		printf("connected with %x, port %d...\n", srv_addr.sin_addr.s_addr, srv_addr.sin_port);
		
		// dispatch to the thread pool the processing of the new connection
		tp_submit(&tpool, dispatch_connection, (void *) (size_t) cfd);
		  
	}
}

int main(int argc, char *argv[]) {
	int sfd;
	
	
	// create server socket
	if ((sfd = create_bind_server_socket(ECHO_SERVER_ADDR, ECHO_SERVER_PORT)) < 0) {
		fprintf(stderr, "error creating socket\n");
		return 1;
	}
 
    // set listen queue size
	if (listen(sfd, BACKLOG) == -1) {
		fprintf(stderr, "error setting  listen queue size\n");
		return 1;
	}
	
	// init tpool
	tp_init(&tpool);
	
	// run the server
	run(sfd);
	
	tp_destroy(&tpool);	 
	close(sfd);
}


 
