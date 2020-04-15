#include <unistd.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <stdio.h>
#include <string.h>


#include "memutils2.h"

 
void * shmem(const char *path, size_t size) {
	
	int fd = shm_open(path, O_CREAT | O_RDWR, 0666);
	 
	if (fd == -1) return NULL;
	
	  
	if (ftruncate(fd, size) == -1) {
		perror("error truncating file!");
		close(fd);
		return NULL;
	}
	 
 
	void *base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (base == (void*) -1) {
		perror("error mapping file!");
		close(fd);
		return NULL;
	}
	
	close(fd);
	return base;
						
}

void *anon_shmem(size_t size) {
	 
	int no_fd = -1;
 
	void *base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, no_fd, 0);
	if (base == (void*) -1) {
		perror("error mapping file!");
		return NULL;
	}
 
	return base;
						
}


 

