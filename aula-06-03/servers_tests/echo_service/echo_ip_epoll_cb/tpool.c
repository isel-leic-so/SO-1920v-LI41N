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
		
		if (item == NULL) break;
		item->func(item->arg); // execute work item!
		
		destroy_wi(item);
		
	}
	return NULL;
}


void tp_init(tpool_t *pool) {
	
	queue_init(&pool->queue);
	pthread_mutex_init(&pool->lock, NULL);
	for(int i=0; i < NTHREADS; ++i) {
		pthread_attr_t attr;
		
		pthread_attr_init(&attr);
		//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&pool->threads[i], &attr, oper_func, pool);
	}
	pool->destroying = 0;
}

void tp_submit(tpool_t *pool, func_t func, void *arg) {
	if (atomic_load(&pool->destroying) != 0) return;
	pthread_mutex_lock(&pool->lock);
	if (atomic_load(&pool->destroying) == 0) {
		workitem_t *wi = create_wi(func, arg);
		queue_put(&pool->queue, wi);
	}
	pthread_mutex_unlock(&pool->lock);
}

void tp_destroy(tpool_t *pool) {
	int expected = 0;
	if (atomic_compare_exchange_strong(&pool->destroying, &expected, 1)) { // first to destroy
		pthread_mutex_lock(&pool->lock);
		
		printf("pool destroy start!\n");
		for(int i=0; i < NTHREADS; ++i) {
		 	queue_put(&pool->queue, NULL);
		}
	
		pthread_mutex_unlock(&pool->lock);
		for(int i=0; i < NTHREADS; ++i) {
			pthread_join(pool->threads[i], NULL);
		}
		
	}
	
	queue_destroy(&pool->queue);
	
	pthread_mutex_destroy(&pool->lock);
	
	printf("pool destroy done!\n");
	
	// to implement
	
}

