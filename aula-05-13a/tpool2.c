#include <stdio.h>
#include <stdlib.h>

#include "tpool2.h"


__attribute__((always_inline))
static inline work_item_t *wi_create(tpool_t *pool, work_func_t func, void *arg) {
	work_item_t * wi;
//	if (is_list_empty(&pool->wi_free_list)) {
//		wi = (work_item_t *) malloc(sizeof(work_item_t));	 
//	}
//	else
//		wi = container_of(remove_head_list(&pool->wi_free_list), work_item_t, link);
    if (is_list_empty(&pool->wi_free_list)) return NULL;
	wi = container_of(remove_head_list(&pool->wi_free_list), work_item_t, link);	
	wi->func = func; wi->arg = arg;
	return wi;
}

__attribute__((always_inline))
static inline void wi_destroy(tpool_t *pool, work_item_t *wi) {
	insert_head_list(&pool->wi_free_list, &wi->link);
	//free(wi);
}


static void init_wi_pool(tpool_t *pool) {
	for(int i=0; i < 1024; ++i) 
		insert_tail_list(&pool->wi_free_list, &pool->freepool[i].link);
}

static void *pool_func(void *arg) {
	tpool_t *pool = (tpool_t*) arg;
	
	pthread_mutex_lock(&pool->lock);
	
	while(1) {
		
		while(is_list_empty(&pool->queue))
			pthread_cond_wait(&pool->hasItems, &pool->lock);
		
		work_item_t *wi = (work_item_t *) 
			remove_head_list(&pool->queue);
		pthread_mutex_unlock(&pool->lock);
			
		
		
		if (wi->func == NULL) break;
		
		wi->func(wi->arg);
		pthread_mutex_lock(&pool->lock);
		wi_destroy(pool, wi);
	
		
	}
	pthread_mutex_unlock(&pool->lock);
	return NULL;
}



void tp_submit(tpool_t *pool, work_func_t func, void *arg) {
	pthread_mutex_lock( &pool->lock);
	work_item_t *wi = wi_create(pool, func, arg);
	insert_tail_list(&pool->queue, &wi->link);
	pthread_cond_signal(&pool->hasItems);
	pthread_mutex_unlock( &pool->lock);
}

void tp_init(tpool_t *pool) {
	init_list_head(&pool->queue);
	init_list_head(&pool->wi_free_list);
	init_wi_pool(pool);
	for(int i=0; i < MAX_THREADS; ++i) {
		pthread_create(&pool->threads[i], NULL, pool_func, pool);
	}
}

void tp_destroy(tpool_t *pool) {
	pthread_mutex_lock( &pool->lock);
	pthread_cond_broadcast(&pool->hasItems);
 
	pthread_mutex_unlock( &pool->lock);
	printf("destroy done\n");
}

