#include <stdlib.h>
#include "queue.h"


typedef struct {
	 LIST_ENTRY link;
	 void *item;
} node_t;

static node_t * create_node(void *item) {
	node_t *node = (node_t*) malloc(sizeof(node_t));
	node->item = item;
	return node;
}

static void destroy_node(node_t *node) {
	free(node);
}

void queue_init(queue_t * q) {
	init_list_head(&q->list);
	pthread_mutex_init(&q->lock, NULL);
	pthread_cond_init(&q->hasItems, NULL);
}

void queue_put(queue_t * q, void * item) {
	pthread_mutex_lock(&q->lock);
	node_t *node = create_node(item);
	insert_tail_list(&q->list, &node->link);
	pthread_cond_signal(&q->hasItems);
	pthread_mutex_unlock(&q->lock);
}

void * queue_get(queue_t * q)  {
	pthread_mutex_lock(&q->lock);
	while(is_list_empty(&q->list))
		pthread_cond_wait(&q->hasItems, &q->lock);
	node_t *node = container_of(remove_head_list(&q->list), node_t, link);
	void *item = node->item;
	destroy_node(node);
	pthread_mutex_unlock(&q->lock);
	return item;
}

void queue_destroy(queue_t * q) {
	pthread_mutex_destroy(&q->lock);
	pthread_cond_destroy(&q->hasItems);
}
