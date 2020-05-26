
#ifndef _TPOOL2_H
#define _TPOOL2_H

#include <pthread.h>

#include "../include/list.h"


#define MAX_THREADS 16



typedef void (*work_func_t)(void *arg) ;

typedef struct _work_item {
	LIST_ENTRY link;
	work_func_t func;
	void *arg;

} work_item_t;


	
typedef struct _tpool {
	LIST_ENTRY wi_free_list;
	work_item_t freepool[1024];
	LIST_ENTRY queue;
	pthread_mutex_t lock;
	pthread_cond_t hasItems;
	pthread_t threads[MAX_THREADS];
} tpool_t;


void tp_submit(tpool_t *pool, work_func_t func, void *arg);
	 

void tp_init(tpool_t * pool);
	 

void tp_destroy(tpool_t *pool);
	 


#endif
