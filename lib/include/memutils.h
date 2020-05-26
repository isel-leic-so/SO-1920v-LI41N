
#ifndef _MEM_UTILS_H
#define _MEM_UTILS_H



/*
 * Create a shared memory region of a specific size
 * 
 * arguments:
 * 		the name(path) and size of shared memory 
 * returns:
 * 		the address of the shared memory region in the process or
 *  	NULL in case of error
 */
void *shmem_create(const char *path, size_t size);


/*
 * Open a shared memory region of a specific size
 * 
 * arguments:
 * 		the name(path) and size of shared memory 
 * returns:
 * 		the address of the shared memory region in the process or
 *  	NULL in case of error
 */
void *shmem_open(const char *path);

/*
 * Creates an anonymous shared memory region for use with
 * descendent related processes.
 * 
 * arguments:
 * 		the size of the region
 * 
 * returns:
 * 		the address of the shared memory region in the process or
 *  	NULL in case of error
 */
void *shmem_anon_create(size_t size);


void shmem_destroy(const char *name, void *base, size_t size);


void shmem_anon_destroy(void *base, size_t size);
	
/*
 * Maps an existent or new file in a memory region of the process.
 * 
 * arguments:
 * 		the name(path) and size of shared memory if we are creating a file
 * returns:
 * 		the address of the shared memory region in the process or
 *  	NULL in case of error
 */
void * map_file(const char *path, size_t newsize);

	
/*
 * unmaps the memory region starting in "base" address
 */
void unmap_file(void *base, int size);

#endif
