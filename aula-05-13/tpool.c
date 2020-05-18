#include <stdlib.h>
#include "tpool.h"

static workitem_t *create_wi(func_t func, void *arg) {
	workitem_t *wi = (workitem_t*) malloc(sizeof(workitem_t));
	wi->func = func;
	wi->arg = arg;
	return wi;
}

static void destroy_wi(workitem_t *wi) {
	free(wi);
}

static void* oper_func(void *arg) {
	tpool_t *pool = (tpool_t*) arg;
	while(1) {
		workitem_t *item = queue_get(&pool->queue);
		
		item->func(item->arg); // execute work item!
		
		destroy_wi(item);
		
	}
	return NULL;
}


void tp_init(tpool_t *pool) {
	queue_init(&pool->queue);
	for(int i=0; i < NTHREADS; ++i) {
		pthread_create(&pool->threads[i], NULL, oper_func, pool);
	}
}

void tp_submit(tpool_t *pool, func_t func, void *arg) {
	workitem_t *wi = create_wi(func, arg);
	queue_put(&pool->queue, wi);
}

void tp_destroy(tpool_t *pool) {
	// to implement
}

