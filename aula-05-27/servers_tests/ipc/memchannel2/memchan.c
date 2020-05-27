#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "memutils.h"
#include "memchan.h"


/*
 * Create a generic synchronized ring buffer in private or shared memory
 */
mem_channel_t *mc_create(int elem_size, int capacity, const char *name) {
	int mc_size = sizeof(mem_channel_t) + elem_size*capacity;
	mem_channel_t *rb;
		
	bool has_items_created = false, has_space_created = false;
	

	rb = (mem_channel_t *) shmem_create(name, mc_size); 
		
	
	if (sem_init(&rb->has_items, true, 0) != 0) goto error;
	has_items_created = true;
	if (sem_init(&rb->has_space, true, capacity) != 0) goto error;
	has_space_created = true;
	rb->elem_size = elem_size;
	rb->capacity = capacity;
	rb->total_size = mc_size;
	rb->iget = rb->iput = 0;
	rb->ownerid = getpid();
	
	return rb;
error:
	if (rb == NULL) return NULL;
	if (has_space_created) sem_destroy(&rb->has_space);
	if (has_items_created) sem_destroy(&rb->has_items);
	shmem_destroy(name, rb, mc_size); 
	return NULL;
}

/*
 * Opens an existing synchronized ring buffer on shared memory
 */
mem_channel_t *mc_open(const char *name) {	
	return shmem_open(name);
}


bool mc_put(mem_channel_t *rb, void *item) {
	// wait for space avaiable
	sem_wait(&rb->has_space);
	long pos = atomic_fetch_add(&rb->iput, 1) % rb->capacity;
	// copy the value to buffer
	memcpy(rb->buffer + pos*rb->elem_size, item, rb->elem_size);
	// notify receivers
	sem_post(&rb->has_items);
	return true;
}

bool mc_get(mem_channel_t *rb, void *item_ptr) {
	// wait for items avaiable
	sem_wait(&rb->has_items);
	long pos = atomic_fetch_add(&rb->iget, 1) % rb->capacity;
	// copy the value to client buffer
	memcpy(item_ptr, rb->buffer + pos*rb->elem_size, rb->elem_size);
	// notify senders
	sem_post(&rb->has_space);
	return true;
}

void mc_destroy(mem_channel_t *rb, const char *name) {
	if (rb == NULL) return;
	if (rb->ownerid != getpid()) {
		munmap(rb, rb->total_size);
		return;
	}
	sem_destroy(&rb->has_space);
	sem_destroy(&rb->has_items);
	shmem_destroy(name, rb, rb->total_size);
}
