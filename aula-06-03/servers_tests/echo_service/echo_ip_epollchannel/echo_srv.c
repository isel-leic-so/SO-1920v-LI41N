/*
 * implements a  stream IP socket echo server using multiplexed I/O via epoll
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <sys/epoll.h>
#include "echo_service.h"

// NO TIMEOUT
#define	 INFINITE -1

// size of pending connections queue
#define BACKLOG 128

// MAX epool ready descriptors
#define EPOOL_MAX_FDS 1024


// globals

struct epoll_event clients[EPOOL_MAX_FDS]; 	// client descriptors
static int nclients;						// total current clients
static int listenfd; 						// listen socket
static int epoll_fd;						// file descriptor


int create_bind_server_socket(const char* ip_addr, int port) {
	int sfd;
	struct sockaddr_in srv_addr;
	
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) return -1;
	
	
	/*  bind socket */
	bzero(&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr(ip_addr);
	srv_addr.sin_port = htons(port);
	
	if (bind(sfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1) {
		close(sfd);
		return -2;
	}
	
	
	return sfd;
}

int init(const char* ip_addr, int port)  {
	struct epoll_event evd; // for listen socket

	listenfd = create_bind_server_socket(ip_addr, port);
	if (listenfd == -1) return -1;
	
	if ((epoll_fd = epoll_create(EPOOL_MAX_FDS)) == -1) {
		close(listenfd);
		return -2;
	}
 
	evd.data.fd = listenfd;
	evd.events = EPOLLIN;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &evd) == -1) {
		close(listenfd);
		return -3;
	}
	
	  // set listen queue size
	if (listen(listenfd, BACKLOG) == -1) {
		fprintf(stderr, "error setting  listen queue size\n");
		close(listenfd);
		return -4;
	}
	
	 
		
	nclients = 1;
	
	return 0;
}

int add_client(int cfd) {
	struct epoll_event evd;  

	evd.data.fd = cfd;
	evd.events = EPOLLIN | EPOLLERR;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cfd, &evd) == -1) return -1;

	nclients++;
	return 0;
}

int rem_client(int cfd) {
	struct epoll_event evd;  

	evd.data.fd = cfd;
	evd.events = EPOLLIN | EPOLLERR;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cfd, &evd) == -1) return -1;

	nclients--;
	return 0;
}


void process_request(int cfd, echo_msg_t *request) {
	// just echo
	write(cfd, request, sizeof(echo_msg_t));
}

void process(int nready) {
	
	for(int i= 0;  i < nready ; ++i) {
		struct epoll_event *evd = clients+i;  
		int cfd; // connection file descriptor
		if (evd->data.fd == listenfd && evd->events & EPOLLIN) { // new connection
			socklen_t clilen = sizeof(struct sockaddr_in);		// client socket addr size
			struct sockaddr_in cliaddr;							// client socket addr
			cfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
			if (cfd == -1) {
				perror("error accepting connection!");
				return;
			}
			printf("connected with %x, port %d...\n", cliaddr.sin_addr.s_addr, cliaddr.sin_port);
		
			if (add_client(cfd) == -1) {
				close(cfd);
				printf("max clients achieved!\n");
				return;
			}
		}
		else if (evd->events & (EPOLLIN | EPOLLERR)) { 
			echo_msg_t request;
			cfd = evd->data.fd;
			
			int nr = read(cfd, &request, sizeof(request));
			if (nr <= 0) {
				 close(cfd);
				 rem_client(cfd);
			}
			else {	
				process_request(cfd, &request);
			}		
		}
			
	}
			
}


void loop() {
	int nready; 
	
	while(true)  {
		nready = epoll_wait(epoll_fd, clients, nclients, INFINITE);
		//printf("there are %d ready descriptors!\n", nready);
		process(nready);	
	}
}



int main(int argc, char *argv[]) {
 
	int res;
	
	if ((res = init(ECHO_SERVER_ADDR, ECHO_SERVER_PORT)) < 0) {
		fprintf(stderr, "error %d starting  service\n", res);
		return 1;
	}
	
	loop();
	return 0;
 
}


 
