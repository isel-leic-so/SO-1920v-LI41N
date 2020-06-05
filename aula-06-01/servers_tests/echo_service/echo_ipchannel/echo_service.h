#pragma once

 

#define ECHO_SERVER_ADDR "127.0.0.1"
#define ECHO_SERVER_PORT 60000

#define MAX_MSG_SIZE 256


typedef struct  {
	char msg[MAX_MSG_SIZE];
} echo_msg_t;

 
