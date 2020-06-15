/*
 * A new version of echo libuv
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <stdbool.h>
#include "echo_service.h"

 
#define DEFAULT_BACKLOG 128

static uv_loop_t loop;	// the libuv loop 
static uv_tcp_t server;	// server socket
static uv_tty_t input;  // for asynchronous stdin access  

static uv_work_t work;	// for submit work on file I/O threadpool

char input_buf[128];	// input buffer 

static int nclients=0;	// toral active connections
bool to_destroy;		// to support controlled termination

// used to define the read buffer for connection socket
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	channel_t *chn = (channel_t*) handle;
    buf->base = chn->buffer;
    buf->len = BUFFER_SIZE;
}

// used to define the read buffer for standard  input
void alloc_buffer2(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = input_buf;
    buf->len = 128;
}

// callback for connection socket close completion
void on_close(uv_handle_t* handle) {
    free(handle);
}

// callback for input close completion
void on_input_close_completion(uv_handle_t* handle) {
    printf("input socket closed!\n");
  
	
}


void write_completion(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
}

void read_stdin_completion(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    printf("read stdin completion!\n");
    printf("nclients = %d\n", nclients);
    if (nclients == 0) {
		uv_close((uv_handle_t*) &input, on_input_close_completion);
		uv_tcp_close_reset(&server, on_input_close_completion);
		
	}
	else {
		to_destroy = true;
	}
    
    //uv_close((uv_handle_t*) &client, on_input_close_completion);
    //uv_tcp_close_reset(&server, on_input_close_completion);
    //uv_close((uv_handle_t*) &server, NULL);
   
}


void read_completion(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	channel_t *chn = (channel_t*) client; 
    if (nread > 0) {
        chn->buf_desc.base = chn->buffer;
        chn->buf_desc.len =  nread;
        uv_write( &chn->req, client, &chn->buf_desc, 1, write_completion);
        return;
    }
    if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
		--nclients;	
		uv_close((uv_handle_t*) client, on_close);
		if (to_destroy && nclients == 0) {
			uv_close((uv_handle_t*) &input, on_input_close_completion);
			uv_tcp_close_reset(&server, on_input_close_completion);
		}

    }
}


// function executed on file I/O threadpool
void work_func(uv_work_t* req) {
	void *arg = req->data;
	
	printf("get %p on thread %ld\n", arg, pthread_self());
}
 
// callback called in loop thread when file I/O threadpool work completion 
void complete_work_cb(uv_work_t* req, int status) {
	printf("received eork completion status %d on thread %ld\n", status, pthread_self());
}


void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }
	
    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(channel_t));
    
    uv_tcp_init(&loop, client);
    
    // to test submitting work to file I/O pool
    printf("loop address %p on loop thread loop %ld\n", &loop, pthread_self());
	work.data = &loop;
	
	// submit work
	uv_queue_work(&loop, &work, work_func, complete_work_cb);
	
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
		nclients++;
        uv_read_start((uv_stream_t*) client, alloc_buffer, read_completion);
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
    uv_read_start((uv_stream_t*) &input, alloc_buffer2, read_stdin_completion);
    
    
    // run the uv loop
    uv_run(&loop, UV_RUN_DEFAULT);
    
    
    return 0;
}
