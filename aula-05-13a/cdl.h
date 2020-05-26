#ifndef _CDL_H
#define _CDL_H

#include <pthread.h>

typedef struct _cdl {
	int counter;
	pthread_mutex_t lock;
	pthread_cond_t done;
} cdl_t;

void cdl_init(cdl_t *cdl, int initial);
	 

void cdl_signal_one(cdl_t *cdl);
 

void cdl_wait_for_all(cdl_t *cdl);

void cdl_destroy(cdl_t *cdl);
 

#endif
