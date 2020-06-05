#include <stdio.h>
#include <stdlib.h>

#include "loop.h"

// globals
struct epoll_event clients[EPOOL_MAX_FDS]; 	// client descriptors
static int nclients;						// total current clients

static int epoll_fd;						// file descriptor

// add a file descriptor to the epoll set
static  int add_client(int cfd, async_handle_t ah) {
	struct epoll_event evd;  

	evd.data.ptr = ah;
	evd.events = EPOLLIN | EPOLLERR;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cfd, &evd) == -1) return -1;

	nclients++;
	return 0;
}


// remove a file descriptor from the epoll set
static  int rem_client(int cfd) {
	struct epoll_event evd;  

	evd.events = EPOLLIN | EPOLLERR;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cfd, &evd) == -1) return -1;
	nclients--;
	return 0;
}


// process the ready handles
static void process(int nready) {
	for(int i= 0;  i < nready ; ++i) {
		struct epoll_event *evd = clients+i;  
		async_handle_t ah; // handle context
	    if (evd->events & (EPOLLIN | EPOLLERR)) { 
			ah = (async_handle_t) evd->data.ptr;
			
			if (ah->cb != NULL) ah->cb(ah, ah->ctx); 
		}	
	}		
}


// the loop!
void ap_loop() {
	int nready; 
	
	for(; ; )  {
		nready = epoll_wait(epoll_fd, clients, nclients, INFINITE);
		//printf("there are %d ready descriptors!\n", nready);
		process(nready);	
	}
}


// async operations

// regist the file descriptor in the loop
async_handle_t ap_regist_fd(int fd) {
	async_handle_t ah = (async_handle_t) malloc(sizeof(struct  async_handle_impl));
	
	ah->fd = fd;
	ah->cb = NULL;
	ah->ctx = NULL;
	
	add_client(fd, ah);
	return ah;
}

// start async operation -just define the associated callback
static void ap_do_async(async_handle_t ah, async_cb cb, void *ctx) {
	ah->cb = cb;
	ah->ctx = ctx;
}

// async accept  
void ap_accept_async(async_handle_t ah, async_cb accept_cb, void *ctx) {
	ap_do_async(ah, accept_cb, ctx);
}

// async read 
void ap_read_async(async_handle_t ah, async_cb read_cb, void *ctx) {
	ap_do_async(ah, read_cb, ctx);
}

// close handle
void ap_close(async_handle_t ah) {
	rem_client(ah->fd);
	free(ah);
}

// create the associated epoll
int ap_init()  {	
	if ((epoll_fd = epoll_create(EPOOL_MAX_FDS)) == -1) {
		return -1;
	}

	nclients = 0;
	return 0;
}

// get the fd assocaited to the passed async_handle_t
int ap_getfd(async_handle_t ah) { return ah->fd; }


