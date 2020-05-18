#ifndef _QUEUE_H
#define _QUEUE_H


#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


#include "../include/types.h"
#include "../include/list.h"


typedef struct {
	LIST_ENTRY list;
	pthread_mutex_t lock;
	pthread_cond_t hasItems; 
} queue_t;


void queue_init(queue_t * q);
void queue_put(queue_t * q, void * item);
void * queue_get(queue_t * q);


#endif

