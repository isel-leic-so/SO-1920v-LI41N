/*
 * implements an echo server using a ring buffer based memory channel
 */

#include <stdio.h>
#include <string.h>
#include "memchan.h"
#include "echo_service.h"


#define MAX_MSGS	1024

int main(int argc, char *argv[]) {
	 
	printf("server: start!\n");
	mem_channel_t *channel = mc_create(sizeof(echo_msg_t), MAX_MSGS, ECHO_SERVICE_NAME);
	if (channel == NULL) {
		fprintf(stderr, "error creating memory channel\n");
		return 1;
	}
	printf("server: after channel creation!\n");
 	 
	while(1) {
		echo_msg_t msg;
		mc_get(channel, &msg);
	
		//printf("server: msg received - %s!\n", msg.msg);
		// send response
		char resp_name[CHANNEL_NAME_MAX_SIZE];
		
		snprintf(resp_name, CHANNEL_NAME_MAX_SIZE, "%s%d", ECHO_CLIENT_PREFIX, msg.sender);
		mem_channel_t *resp_channel = mc_open(resp_name);
		if (resp_channel == NULL) {
			fprintf(stderr, "error opening client channel %s\n", resp_name);
			break;
		}
		
		mc_put(resp_channel, &msg);
		mc_destroy(resp_channel, resp_name);

	}
	
	mc_destroy(channel, ECHO_SERVICE_NAME);
	
	printf("server done!\n");
	return 0;
}
