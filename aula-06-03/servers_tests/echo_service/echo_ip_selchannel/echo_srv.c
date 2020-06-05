/*
 * implements IP stream socket based echo server using multiplexed I/O with select
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
 

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#include <sys/select.h>
#include <sys/time.h>

#include "echo_service.h"


// size of pending connections queue
#define BACKLOG 5


// globals

static int clients[FD_SETSIZE]; 	// client descriptors
static int maxclient_fd;			// max client descriptor
static int maxclients;				// max simultaneous clients
static int nclients;				// total current clients;

static int listenfd; 				// listen socket

static fd_set rset, allset;			// used descriptors bitmaps



int create_bind_server_socket(const char* ip_addr, int port) {
	int sfd;
	struct sockaddr_in srv_addr;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) return -1;
	
 
	/*  bind socket */
	bzero(&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = INADDR_ANY; // inet_addr(ip_addr);
	srv_addr.sin_port = htons(port);
	
	if (bind(sfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1) {
		close(sfd);
		return -3;
	}
	
	return sfd;
}

bool init(const char* ip_addr, int port)  {
	
	listenfd = create_bind_server_socket(ip_addr, port);
	if (listenfd == -1) return false;
	
	  // set listen queue size
	if (listen(listenfd, BACKLOG) == -1) {
		fprintf(stderr, "error setting  listen queue size\n");
		close(listenfd);
		return false;
	}
	
	 
	maxclients = FD_SETSIZE - 4;
	maxclient_fd = listenfd;
	nclients = 0;

	return true;
}

int add_client(int cfd) {
	if (nclients == maxclients) return -1;
	 
	clients[nclients++] = cfd;
	if (cfd > maxclient_fd) maxclient_fd = cfd;
	return 0;
}

void rem_client(int cfd, int i) {
	close(cfd);
	clients[i] = clients[--nclients];
	FD_CLR(cfd, &allset);
}


void process_request(int cfd, echo_msg_t *request) {
	// just echo
	write(cfd, request, sizeof(echo_msg_t));
}

void process(int nready) {
	int cfd; // connection file descriptor
	 
	if (FD_ISSET(listenfd, &rset)) { // new connection
		socklen_t clilen = sizeof(struct sockaddr_in);	// client socket addr size
		struct sockaddr_in cliaddr;						// client socket addr
		cfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
		if (cfd == -1) {
			perror("error accepting connection!");
			return;
		}
		printf("connected with %x, port %d...\n", cliaddr.sin_addr.s_addr, cliaddr.sin_port);
	
		if (nclients == maxclients || add_client(cfd) == -1) {
			close(cfd);
			printf("max clients achieved!\n");
			return;
		}
		FD_SET(cfd, &allset);
		if (--nready == 0) return;
	}
	for(int i=0; i < nclients && nready > 0; ++i) {
		if ((cfd = clients[i]) != -1 && FD_ISSET(cfd, &rset)) { 
			
			echo_msg_t request;	// request msg
			// printf("data avaiable on socket %d!\n", cfd);
			
			int nr = read(cfd,&request, sizeof(request));
			if (nr <= 0) {
				printf("end of connection!\n");
				rem_client(cfd, i);
				--nready;
			}
			else {	
				process_request(cfd, &request);
			}		
		}	
	}		
}

void loop() {
	int nready; 
	
	FD_ZERO(&allset);
	FD_ZERO(&rset);
	FD_SET(listenfd, &allset);
	
	while(true)  {
		rset = allset;
		nready = select(maxclient_fd +1, &rset, NULL, NULL, NULL);
		// printf("there are %d ready descriptors!\n", nready);
		process(nready);	
	}	
}



int main(int argc, char *argv[]) {
	
	if (!init(ECHO_SERVER_ADDR, ECHO_SERVER_PORT)) {
		fprintf(stderr, "error starting  service\n");
		return 1;
	}
	
	// start the event loop
	loop();
	
	return 0;
 
}


 
