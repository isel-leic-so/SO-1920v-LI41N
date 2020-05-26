#include <stdlib.h>
#include "tpool.h"

static workitem_t *create_wi(func_t func, void *arg) {
	workitem_t *wi = (workitem_t*) malloc(sizeof(workitem_t));
	wi->func = func;
	wi->arg = arg;
	return wi;
}

static void destroy_wi(workitem_t *wi) { free(wi); }

static void* oper_func(void *arg) {
	tpool_t *pool = (tpool_t*) arg;
	while(!pool->terminated) {
		workitem_t *item = queue_get(&pool->queue);
		if (item == NULL) break;
		item->func(item->arg); // execute work item!
		destroy_wi(item);
	}
	return NULL;
}

void tp_destroy(tpool_t *pool) {
	// to implement
	pthread_mutex_lock(&pool->lock);
	if (!pool->terminated) {
		pool->terminated = true;
		// 
		for(int i=0; i < NTHREADS; ++i) {
			queue_put(&pool->queue, NULL);
		}
		
		for(int i=0; i < NTHREADS; ++i) {
			pthread_join(pool->threads[i], NULL);
		}
	}
	pthread_mutex_unlock(&pool->lock);
	
	// free other resources
	queue_destroy(&pool->queue);
	pthread_mutex_destroy(&pool->lock);
	
}


void tp_init(tpool_t *pool) {
	pthread_mutex_init(&pool->lock, NULL);
	queue_init(&pool->queue);
	pool->terminated = false;
	for(int i=0; i < NTHREADS; ++i) {
		pthread_create(&pool->threads[i], NULL, oper_func, pool);
	}
}

void tp_submit(tpool_t *pool, func_t func, void *arg) {
	pthread_mutex_lock(&pool->lock);
	if (!pool->terminated) {
		workitem_t *wi = create_wi(func, arg);
		queue_put(&pool->queue, wi);
	}
	pthread_mutex_unlock(&pool->lock);
}


