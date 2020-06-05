#pragma once


#include <sys/types.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdbool.h>

#define CHANNEL_NAME_MAX_SIZE 128

typedef unsigned char byte;

typedef struct {
	pid_t ownerid;
	sem_t has_items;
	sem_t has_space;
	int elem_size;
	int capacity;
	int total_size;
	atomic_long iget;
	atomic_long iput;
	byte buffer[1];
} mem_channel_t;



/*
 * Create a generic synchronized ring buffer in private or shared memory
 */
mem_channel_t *mc_create(int elem_size, int capacity, const char *name);
	 

/*
 * Opens an existing synchronized ring buffer on shared memory
 */
mem_channel_t *mc_open(const char *name);
	 

bool mc_put(mem_channel_t *rb, void *item);
	 

bool mc_get(mem_channel_t *rb, void *item_ptr);
	

void mc_destroy(mem_channel_t *rb, const char *name);
 
