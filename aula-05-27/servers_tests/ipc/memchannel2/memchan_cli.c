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
	
	mem_channel_t *channel = mc_open(argv[1]);
	if (channel == NULL) {
		fprintf(stderr, "error creating memory channel\n");
		return 1;
	}
	
	char msg[MAX_MSG_SIZE];
	
	strcpy(msg, "Hello");  mc_put(channel, msg);
	strcpy(msg, "Test");  mc_put(channel, msg);
	strcpy(msg, ""); mc_put(channel, msg);
	
	 
 
	
	mc_destroy(channel);
	
	printf("client done!\n");
	return 0;
}
