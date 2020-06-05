#pragma once


#include <sys/types.h>
#include <stdbool.h>

#define CHANNEL_NAME_MAX_SIZE 128

typedef unsigned char byte;

typedef struct {
	int iput, iget;			// read write indexes
	int capacity; 			// number of elements in buffer
	int elem_size; 			// element size
	int size;				// used number of elements
	pid_t owner;			// creator process id
	pthread_mutex_t lock;	
	pthread_cond_t hasItems;// to notify the existence of avaiable items
	pthread_cond_t hasSpace;// to notify the existente of avaiable space		
	byte buf[1];
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
	

void mc_destroy(mem_channel_t *rb,const char *name);
 
