/*
 * implements an echo server using a callback mode supoported bay epoll
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "echo_service.h"
#include "loop.h"

// size of pending connections queue
#define BACKLOG 128


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

async_handle_t server_init() {
	int listenfd = create_bind_server_socket(ECHO_SERVER_ADDR, ECHO_SERVER_PORT);
	if (listenfd == -1) return NULL;
	
	async_handle_t listen_ah = ap_regist_fd(listenfd);
	
	  // set listen queue size
	if (listen(listenfd, BACKLOG) == -1) {
		fprintf(stderr, "error setting  listen queue size\n");
		ap_close(listen_ah);
		return NULL;
	}
	return listen_ah;
}


// callback for asynchronous read completion
void on_read_request(async_handle_t ah, void *ctx) {
	echo_msg_t request;	// request msg
	int cfd = ap_getfd(ah);
	int res = read(cfd, &request, sizeof(echo_msg_t));
	if (res <= 0) ap_close(ah);
	else write(cfd, &request, sizeof(echo_msg_t));
}

// callback for asynchronous accept completion	
void on_new_connection(async_handle_t ah, void *ctx) {
	socklen_t clilen = sizeof(struct sockaddr_in);	// client socket addr size
    struct sockaddr_in cliaddr;			// client socket addr
	 
  
	int cfd = accept(ap_getfd(ah), (struct sockaddr *) &cliaddr, &clilen);
	if (cfd > 0) {
		printf("connected with %x, port %d...\n", cliaddr.sin_addr.s_addr, cliaddr.sin_port);
		async_handle_t client = ap_regist_fd(cfd);
		
		// start client read
		ap_read_async(client, on_read_request, NULL);
	}
}



int main(int argc, char *argv[]) {
	// initialize loop infrastructure
	if (ap_init() < 0) {
		fprintf(stderr, "error starting loop\n");
		return 1;
	}
	
	// start the accept call
	ap_accept_async(server_init(), on_new_connection, NULL);
	
	// start the loop
	ap_loop();
	
	return 0;
 
}


 
