#ifndef _QUEUE_H
#define _QUEUE_H


#include "../include/types.h"
#include "../include/usynch.h"
#include "../include/list.h"


#define CAPACITY 1

typedef struct {
	LIST_ENTRY list;
	SEMAPHORE hasItems, hasSpace;
} QUEUE, *PQUEUE;


void queue_init(PQUEUE q);
void queue_put(PQUEUE q, void * item);
void * queue_get(PQUEUE q);
BOOL queue_is_empty(PQUEUE q);

#endif
