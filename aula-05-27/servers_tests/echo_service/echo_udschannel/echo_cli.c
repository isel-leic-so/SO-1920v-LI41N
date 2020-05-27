/*
 * implements an echo server using a ring buffer based memory channel
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "chrono.h"
#include "echo_service.h"
 
#define MAX_SOCK_NAME 256
#define CLIENT_SOCK_PREFIX "sock_client_"

#define NITERS 10000

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


 

int main(int argc, char *argv[]) {
	 
	struct sockaddr_un srv_address;
	int cfd;
	char sock_name[MAX_SOCK_NAME];
	

	// create and bind client socket
	snprintf(sock_name, MAX_SOCK_NAME- 1, "%s%d", CLIENT_SOCK_PREFIX, getpid());
	
	if ((cfd = create_bind_socket(sock_name)) < 0) {
		fprintf(stderr, "error creating socket\n");
		return 1;
	}
	
	 		
	/* Construct server address, and make the connection */
	memset(&srv_address, 0, sizeof(struct sockaddr_un));
	srv_address.sun_family = AF_UNIX;
	strncpy(srv_address.sun_path, ECHO_SERVICE_NAME, sizeof(srv_address.sun_path) - 1);
	
	if (connect(cfd, (struct sockaddr *) &srv_address, sizeof(struct sockaddr_un)) == -1) {
		fprintf(stderr, "Error connecting socket\n");
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
 	//unlink(sock_name);
	printf("client done!\n");
	return 0;
}
