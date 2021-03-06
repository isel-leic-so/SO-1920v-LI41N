#ifndef _TPOOL_H
#define _TPOOL_H

#include <pthread.h>
#include "queue.h"
#include <stdbool.h>

#define NTHREADS 8

typedef void (*func_t)(void *arg);

typedef struct {
	func_t func;
	void *arg;
} workitem_t;

typedef struct {
	pthread_t threads[NTHREADS];
	queue_t queue;
	bool terminated;
	pthread_mutex_t lock;
} tpool_t;



void tp_init(tpool_t *pool);

void tp_submit(tpool_t *pool, func_t func, void *arg);
	 
void tp_destroy(tpool_t *pool);


#endif
