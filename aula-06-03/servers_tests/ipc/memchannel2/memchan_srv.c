/*
 * implements an echo server using a ring buffer based memory channel
 */

#include <stdio.h>
#include <string.h>
#include "memchan.h"

#define MAX_MSG_SIZE 128
#define MAX_MSGS	1024

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: memchan_srv <channel name>\n");
		return 1;
	}
	
	mem_channel_t *channel = mc_create(MAX_MSG_SIZE, MAX_MSGS, argv[1]);
	if (channel == NULL) {
		fprintf(stderr, "error creating memory channel\n");
		return 1;
	}
	
	char msg[MAX_MSG_SIZE];
	
	 
	 
	while(1) {
		mc_get(channel, msg);
		if (msg[0]==0) break;
		printf("%s\n", msg);
	}
	
	mc_destroy(channel);
	
	printf("server done!\n");
	return 0;
}
