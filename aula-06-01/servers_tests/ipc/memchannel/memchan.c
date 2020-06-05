#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include "memutils.h"
#include "memchan.h"


/*
 * Create a generic synchronized ring buffer in private or shared memory
 */
mem_channel_t *mc_create(int elem_size, int capacity, const char *name) {
	
	mem_channel_t *rb = NULL;
	
	int capacity_in_bytes = elem_size*capacity;
	int total_size_in_bytes = capacity_in_bytes + sizeof(mem_channel_t);
	
	if ((rb = (mem_channel_t *) shmem_create(name,  total_size_in_bytes)) == NULL) 
		goto error;
	
	rb->elem_size = elem_size;
	rb->capacity = capacity;
	rb->iput = rb->iget = 0;
	rb->size = 0;
	
	rb->owner = getpid();
	pthread_mutexattr_t attr;
	
	
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED /*shared between processes */);
	pthread_mutex_init(&rb->lock, &attr);
	
	pthread_condattr_t attrc;
	pthread_condattr_init(&attrc);
	pthread_condattr_setpshared(&attrc, PTHREAD_PROCESS_SHARED);
	
	pthread_cond_init(&rb->hasItems, &attrc);
	
	pthread_cond_init(&rb->hasSpace, &attrc);
	
	return rb;
	
	// to complete
error:
	
	return NULL;
}

/*
 * Opens an existing synchronized ring buffer on shared memory
 */
mem_channel_t *mc_open(const char *name) {	
	 
	return (mem_channel_t *) shmem_open(name);
}


bool mc_put(mem_channel_t *rb, void *item) {
	// to complete
	
	// printf("channel: put started!\n");
	pthread_mutex_lock(&rb->lock);
	
	// printf("channel: put acquired lock!\n");
	while(rb->size == rb->capacity) 
		pthread_cond_wait(&rb->hasSpace, &rb->lock);
	
	memcpy(rb->buf + rb->iput*rb->elem_size, item, rb->elem_size);
	rb->iput++;
	if (rb->iput == rb->capacity) rb->iput = 0;
	rb->size++;

	pthread_mutex_unlock(&rb->lock);
	if (pthread_cond_signal(&rb->hasItems) != 0)
		printf("error signaling condition hasItems");
	// printf("channel: put end!\n");
	
	return true;
}


bool mc_get(mem_channel_t *rb, void *item_ptr) {
	// printf("channel: get started!\n");
	pthread_mutex_lock(&rb->lock);
	
	// printf("channel: get acquired lock!\n");
	while(rb->size == 0) 
		pthread_cond_wait(&rb->hasItems, &rb->lock);
	
	
	memcpy(item_ptr, rb->buf + rb->iget*rb->elem_size,  rb->elem_size);
	rb->iget++;
	if (rb->iget == rb->capacity) rb->iget = 0;
	rb->size--;
	
	pthread_mutex_unlock(&rb->lock);
	
	if (pthread_cond_signal(&rb->hasSpace) != 0)
		printf("error signaling condition hasSpace");
	// printf("channel: get end!\n");
	return true;
}

void mc_destroy(mem_channel_t *rb, const char *name) {
	int capacity_in_bytes = rb->elem_size*rb->capacity;
	int total_size_in_bytes = capacity_in_bytes + sizeof(mem_channel_t);
	if (getpid() == rb->owner) {
		
		pthread_cond_destroy(&rb->hasItems);
		pthread_cond_destroy(&rb->hasSpace);
		pthread_mutex_destroy(&rb->lock);
		shmem_destroy(name, rb, total_size_in_bytes);
	
	}
	else {
		munmap(rb, total_size_in_bytes);
	}
}
