#pragma once

#define ECHO_SERVICE_NAME "echo_memchannel_service"

#define ECHO_CLIENT_PREFIX  "client_"
#define MAX_MSG_SIZE 256



#include "memchan.h"

typedef struct  {
	int sender;
	char msg[MAX_MSG_SIZE];
} echo_msg_t;


