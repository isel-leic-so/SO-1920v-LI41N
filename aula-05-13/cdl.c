#include <pthread.h>
#include "cdl.h"

 

void cdl_init(cdl_t *cdl, int initial) {
	atomic_store(&cdl->counter, initial);
	//cdl->counter = initial;
	pthread_mutex_init(&cdl->lock, NULL);
	pthread_cond_init(&cdl->done, NULL);
}


void cdl_signal_one(cdl_t *cdl) {
	if (atomic_fetch_sub(&cdl->counter, 1) > 1) return;
	pthread_mutex_lock(&cdl->lock);
	pthread_cond_broadcast(&cdl->done);
	pthread_mutex_unlock(&cdl->lock);
}

void cdl_wait_for_all(cdl_t *cdl) {
	pthread_mutex_lock(&cdl->lock);
	
	while (atomic_load(&cdl->counter) > 0)
		pthread_cond_wait(&cdl->done, &cdl->lock);
	pthread_mutex_unlock(&cdl->lock);
}

void cdl_destroy(cdl_t *cdl) {
	pthread_mutex_destroy(&cdl->lock);
	pthread_cond_destroy(&cdl->done);
}


