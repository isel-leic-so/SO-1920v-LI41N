
#include <sys/epoll.h>


// NO TIMEOUT
#define	 INFINITE -1

// MAXIMUM READY HANDLES
#define EPOOL_MAX_FDS 1024


// async handles

struct async_handle_impl;

typedef struct async_handle_impl *async_handle_t;

typedef struct  {
	union {
		int res_fd;
		int nbytes;
	};
	int status;
} async_result_t;

typedef void (*async_cb)(async_handle_t ash,  void *ctx);


struct  async_handle_impl {
	int fd; // associated file descriptor
	async_cb cb;
	void *ctx;
};


	 


// async operations


int ap_init();
	 
async_handle_t ap_regist_fd(int fd);

int ap_getfd(async_handle_t ah); 

void ap_accept_async(async_handle_t ah, async_cb accept_cb, void *ctx);

void ap_read_async(async_handle_t ah, async_cb read_cb, void *ctx);
 
void ap_close(async_handle_t ah);
	 

void ap_loop();

