
#ifndef _MEM_UTILS_H
#define _MEM_UTILS_H


#include <sys/mman.h>

 

void *shmem(const char *path, size_t size);
void *anon_shmem(size_t size);
	
#endif
