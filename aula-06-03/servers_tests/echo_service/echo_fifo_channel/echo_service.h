#pragma once

#define CHANNEL_NAME_MAX_SIZE 256
#define ECHO_SERVICE_NAME "echo_fifo_service"

#define ECHO_CLIENT_PREFIX  "client_"
#define MAX_MSG_SIZE 256



typedef struct  {
	int sender;
	char msg[MAX_MSG_SIZE];
} echo_msg_t;


