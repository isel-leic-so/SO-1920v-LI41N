/*
 * implements an echo client using a stream IP socket 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
 

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "chrono.h"
#include "echo_service.h"
 
#define MAX_SOCK_NAME 256
#define CLIENT_SOCK_PREFIX "sock_client_"

#define NITERS 10000

int create_client_socket() {
	return socket(AF_INET, SOCK_STREAM, 0); 
}


int main(int argc, char *argv[]) {
	 
	struct sockaddr_in srv_addr;
	int cfd;
	 
	if ((cfd = create_client_socket()) < 0) {
		fprintf(stderr, "error creating socket\n");
		return 1;
	}
	
	 		
	/* Construct server address, and make the connection */
	bzero(&srv_addr, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr(ECHO_SERVER_ADDR);
	srv_addr.sin_port = htons(ECHO_SERVER_PORT);
		
	if (connect(cfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1) {
		perror("Error connecting socket\n");
		return 2;
	}
	
	echo_msg_t msg, resp;
	int pid  = getpid();
	
	chrono_t chron = chrono_start();
	 

	for(int try=1; try <= NITERS; try++) {
		sprintf(msg.msg, "Hello_%d_%d", pid, try);
		write(cfd, &msg, sizeof(echo_msg_t));
		// get response
		read(cfd, &resp, sizeof(echo_msg_t));
		//printf("%s\n", resp.msg);
	}
	
	printf("%d tries in %ld micros!\n", NITERS, chrono_micros(chron));
	
	close(cfd);
  	
	printf("client done!\n");
	return 0;
}
