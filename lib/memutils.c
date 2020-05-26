#include <unistd.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <stdio.h>
#include <string.h>


#include "include/memutils.h"


static void *shmem_internal_map(int fd, size_t size) {
	void *base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (base == (void*) -1) {
		close(fd);
		return NULL;
	}
	close(fd);
	return base;
}


/*
 * Creat/open a shared memory region of a specific size
 * 
 * arguments:
 * 		the name(path) and size of shared memory 
 * returns:
 * 		the address of the shared memory region in the process or
 *  	NULL in case of error
 */
void * shmem_create(const char *path, size_t size) {
	
	int fd = shm_open(path, O_CREAT | O_RDWR, 0666);
	 
	if (fd == -1) return NULL;
	
	  
	if (ftruncate(fd, size) == -1) {
		perror("error truncating file!");
		close(fd);
		return NULL;
	}
	
	return shmem_internal_map(fd, size);
 					
}

/*
 * opens a shared memory region of a specific size
 * 
 * arguments:
 * 		the name(path) and size of shared memory 
 * returns:
 * 		the address of the shared memory region in the process or
 *  	NULL in case of error
 */
void *shmem_open(const char *path) {
	
	int fd = shm_open(path, O_RDWR, 0666);
	struct stat statbuf;
	
	
	if (fd == -1) return NULL;


	if (fstat(fd, &statbuf) != 0) {
		close(fd);
		return NULL;
	}
	
	
	return shmem_internal_map(fd, statbuf.st_size);	
}

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
void *shmem_anon_create(size_t size) {
	 
	int no_fd = -1;
 
	void *base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, no_fd, 0);
	if (base == (void*) -1) {
		perror("error mapping file!");
		return NULL;
	}
 
	return base;
						
}


void shmem_destroy(const char *name, void *base, size_t size) {
	munmap(base, size);
	unlink(name);
}

void shmem_anon_destroy(void *base, size_t size) {
	munmap(base, size);
}



/*
 * Maps an existent or new file in a memory region of the process.
 * 
 * arguments:
 * 		the name(path) and size of shared memory if we are creating a file
 * returns:
 * 		the address of the shared memory region in the process or
 *  	NULL in case of error
 */
void * map_file(const char *path, size_t newsize) {
	int fd = open(path, O_CREAT | O_RDWR, 0666);
	if (fd == -1) return NULL;
	
	// get the file size;
	struct stat stat;
	
	if (fstat(fd, &stat) == -1)
		return NULL;
	
	if (newsize != 0) {
		if (ftruncate(fd, newsize) == -1) {
			perror("error truncating file!");
			return NULL;
		}
	}
	else newsize = stat.st_size;
	
 
	void *base = mmap(NULL, newsize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (base == (void*) -1) {
		perror("error mapping file!");
		return NULL;
	}
		
	close(fd);
	
	return base;
						
}

/*
 * unmaps the memory region starting in "base" address
 */
void unmap_file(void *base, int size) {
	munmap( base, size);
}
 

