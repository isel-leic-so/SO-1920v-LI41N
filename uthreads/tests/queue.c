 
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "queue.h"

typedef struct {
	LIST_ENTRY link;
	void *item;
} QNODE, *PQNODE;

static PQNODE create_node(void* item) {
	PQNODE n = (PQNODE)malloc(sizeof(QNODE));
	n->item = item;
	return n;
}

static void destroy_node(PQNODE n) {
	free(n);
}


void  queue_init(PQUEUE q, int size) {
	init_list_head(&q->list);
 
	
	sem_init(&q->hasItems, 0, size);
	sem_init(&q->hasSpace, size, size);
}


void queue_put(PQUEUE q, void * item) {
	PQNODE node = create_node(item);
	
	sem_wait(&q->hasSpace, 1);

	insert_tail_list(&q->list, &node->link);
	sem_post(&q->hasItems, 1);

}

void* queue_get(PQUEUE q) {
	
	sem_wait(&q->hasItems, 1);
	 
	PQNODE node = (PQNODE)remove_head_list(&q->list);
	void * val = node->item;
	destroy_node(node);
	
	sem_post(&q->hasSpace, 1);
	return val;
}


BOOL queue_is_empty(PQUEUE q) {
	return is_list_empty(&q->list);
}
