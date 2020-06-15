#pragma once

#include <uv.h>

#define ECHO_SERVER_ADDR "127.0.0.1"
#define ECHO_SERVER_PORT 60000

#define MAX_MSG_SIZE 256

#define BUFFER_SIZE 4096

typedef struct  {
	char msg[MAX_MSG_SIZE];
} echo_msg_t;


typedef struct {
	uv_tcp_t base;
	char buffer[BUFFER_SIZE];
	uv_buf_t buf_desc;
	
	uv_write_t req;
	
} channel_t;

 
