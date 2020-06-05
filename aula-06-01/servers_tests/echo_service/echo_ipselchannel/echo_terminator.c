/*
 * implements an echo server using a ring buffer based memory channel
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "memchan.h"
#include "echo_service.h"
 
#define MAX_MSGS	1
#define NMSGS 10000

int main(int argc, char *argv[]) {
	 
	
	mem_channel_t *channel = mc_open(ECHO_SERVICE_NAME);
	if (channel == NULL) {
		fprintf(stderr, "error opening server channel\n");
		return 1;
	}
	
	// create response channel
	char resp_channel_name[CHANNEL_NAME_MAX_SIZE];
	snprintf(resp_channel_name, CHANNEL_NAME_MAX_SIZE, "%s%d", ECHO_CLIENT_PREFIX, getpid());
	mem_channel_t *resp_channel = mc_create(sizeof(echo_msg_t), 1, resp_channel_name);
	if (channel == NULL) 
		fprintf(stderr, "error creating response channel\n");
	else {	
		
		echo_msg_t msg, resp;
		
		msg.sender = getpid();
		msg.msg[0] = 0;
		 
		mc_put(channel, &msg);
		// get response
		mc_get(resp_channel, &resp);
	}
			 
 
	mc_destroy(resp_channel);
	mc_destroy(channel);
	
	printf("termination done!\n");
	return 0;
}

