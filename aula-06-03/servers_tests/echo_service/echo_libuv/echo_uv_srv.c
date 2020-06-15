#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <stdbool.h>
#include "echo_service.h"

 
#define DEFAULT_BACKLOG 128

static uv_loop_t loop;
static uv_tcp_t server;	
static uv_tty_t input;   

char input_buf[128];
static int nclients=0;
bool to_destroy;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	channel_t *chn = (channel_t*) handle;
    buf->base = chn->buffer;
    buf->len = BUFFER_SIZE;
}

 
void alloc_buffer2(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = input_buf;
    buf->len = 128;
}

void on_close(uv_handle_t* handle) {
    free(handle);
}

void on_close2(uv_handle_t* handle) {
    printf("accept or input socket closed!\n");
}


void echo_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
}

void echo_read_stdin(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	printf("on callback: input buf address=%p\n", input_buf);
    printf("echo read stdin!\n");
    to_destroy = true;
    //uv_close((uv_handle_t*) &client, on_close2);
    //uv_tcp_close_reset(&server, on_close2);
    //uv_close((uv_handle_t*) &server, NULL);
   
}


void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	channel_t *chn = (channel_t*) client; 
    if (nread > 0) {
        chn->buf_desc.base = chn->buffer;
        chn->buf_desc.len =  nread;
        uv_write( &chn->req, client, &chn->buf_desc, 1, echo_write);
        return;
    }
    if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
		--nclients;
		
		uv_close((uv_handle_t*) client, on_close);
		if (to_destroy && nclients == 0) {
			uv_close((uv_handle_t*) &input, on_close2);
			uv_tcp_close_reset(&server, on_close2);
		}
		
		//free(client); 
    }
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }
	
    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(channel_t));
    
    uv_tcp_init(&loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
		printf("loop address %p on loop thread loop %ld\n", &loop, pthread_self());
		nclients++;
        uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);
    }
    else {
        uv_close((uv_handle_t*) client, on_close);
    }

}


int main() {
	struct sockaddr_in addr;
	
    uv_loop_init(& loop);

    printf("input buf address=%p\n", input_buf);
    uv_tcp_init(&loop, &server);

    uv_ip4_addr(ECHO_SERVER_ADDR, ECHO_SERVER_PORT, &addr);

    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }
    
	// For asynchronous input receiving via uv loop
    
    uv_tty_init(&loop, &input, 0, 0);
    uv_read_start((uv_stream_t*) &input, alloc_buffer2, echo_read_stdin);
    
    
    // run the uv loop
    uv_run(&loop, UV_RUN_DEFAULT);
    
    
    return 0;
}
