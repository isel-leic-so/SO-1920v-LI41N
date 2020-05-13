#include <pthread.h>

typedef struct _cdl {
	int counter;
	pthread_mutex_t lock;
	pthread_cond_t done;
} cdl_t;

void cdl_init(cdl_t *cdl, int initial) {
	cdl->counter = initial;
	pthread_mutex_init(&cdl->lock, NULL);
	
	pthread_cond_init(&cdl->done, NULL);
}


void cdl_signal_one(cdl_t *cdl) {
	pthread_mutex_lock(&cdl->lock);
	
	if (--cdl->counter == 0)
		pthread_cond_broadcast(&cdl->done);
	pthread_mutex_unlock(&cdl->lock);
}

void cdl_wait_for_all(cdl_t *cdl) {
	pthread_mutex_lock(&cdl->lock);
	
	while (cdl->counter > 0)
		pthread_cond_wait(&cdl->done, &cdl->lock);
	pthread_mutex_unlock(&cdl->lock);
}


