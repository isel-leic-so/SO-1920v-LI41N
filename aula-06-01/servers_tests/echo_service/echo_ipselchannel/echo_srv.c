/*
 * implements an echo server using a ring buffer based memory channel
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

#include <sys/select.h>

#include "types.h"
#include "echo_service.h"
#include "tpool.h"



// size of pending connections queue
#define BACKLOG 5



// globals

static int clients[FD_SETSIZE]; 	// client descriptors
static int maxclient_fd;			// max client descriptor
static int maxclient_idx;			// max client index
static int maxclients;				// max simultaneous clients
static int nclients;				// total current clients;

static int listenfd; 				// listen socket

static socklen_t clilen;			// client socket addr size
static struct sockaddr_in cliaddr;	// client socket addr
static echo_msg_t request;			// request msg

int create_bind_server_socket(const char* ip_addr, int port) {
	int sfd;
	struct sockaddr_in srv_addr;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) return -1;
	
 
	/*  bind socket */
	bzero(&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = INADDR_ANY; // inet_addr(ip_addr);
	srv_addr.sin_port = port;
	
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
	for(int i=0; i < FD_SETSIZE; ++i)
		clients[i] = -1;
	maxclients = FD_SETSIZE - 4;
	maxclient_idx = 0;
	maxclient_fd = listenfd;
	nclients = 0;
	clilen = sizeof(struct sockaddr_in);
	return true;
}

int add_client(int cfd) {
	for (int i=0; i < FD_SETSIZE; ++i) {
		if (clients[i] == -1) {
			clients[i] = cfd;
			if (i > maxclient_idx) maxclient_idx = i;
			if (cfd > maxclient_fd) maxclient_fd = cfd;
			return i;
		}
	}
	return -1;
}


void process_request(int cfd) {
	// just echo
	write(cfd, &request, sizeof(request));
}

void process(fd_set *rset, fd_set *allset, int nready) {
	int cfd; // connection file descriptor
	 
	if (FD_ISSET(listenfd, rset)) { // new connection
		printf("new connection!\n");
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
		FD_SET(cfd, allset);
		if (--nready == 0) return;
	}
	for(int i=0; i <= maxclient_idx && nready > 0; ++i) {
		if ((cfd = clients[i]) != -1 && FD_ISSET(cfd, rset)) { 
			printf("data avaiable on socket %d!\n", cfd);
			// data avaiable from connection
			// connection closed
			int nr = read(cfd,&request, sizeof(request));
			if (nr <= 0) {
				printf("end of connection!\n");
				close(cfd);
				clients[i] = -1;
				FD_CLR(cfd, allset);
				--nready;
			}
			else {	
				process_request(cfd);
			}		
		}
			
	}
			
}

void loop() {
	int nready; 
	fd_set rset, allset;
	
	FD_ZERO(&allset);
	FD_ZERO(&rset);
	FD_SET(listenfd, &allset);
	
	while(true)  {
		rset = allset;
		nready = select(maxclient_fd +1, &rset, NULL, NULL, NULL);
		printf("there are %d ready descriptors!\n", nready);
		process(&rset, &allset, nready);
		
	}
	
}



int main(int argc, char *argv[]) {
	
	if (!init(ECHO_SERVER_ADDR, ECHO_SERVER_PORT)) {
		fprintf(stderr, "error starting  service\n");
		return 1;
	}
	
	
	loop();
	
	return 0;
 
}


 
