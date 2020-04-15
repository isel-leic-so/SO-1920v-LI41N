
/*-----------------------------------------------------------------
 * This program illustrates the behaviour of a file map operation
 * by showing several virtual memory stats at selected phases in
 * program execution
 * 
 * Build:
 * 		gcc -o maptest -Wall maptest.c ../utils/memutils.c
 * 
 * Isel, 2020
 *----------------------------------------------------------------*/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../utils/memutils.h"
#include "../utils/memutils2.h"

#define SHMEM_SIZE (1000*1000*sizeof(int))

long sum_ints(int *ints, int len) {
	long sum =0;
	for(int i=0; i < len;++i) {
		sum += ints[i];
	}
	return sum;
}

void write_ints(int *ints, int len) {
	for(int i=0; i < len;++i) {
		ints[i] = 1;
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: smem <name> n");
		return 1;
	}
	
	
	char *name = argv[1];
	 
	show_avail_mem("Inicio");
	phase_start("file map");
	
	void *smem = shmem(name, SHMEM_SIZE);
	if (smem == NULL) {
		printf("error  mapping the file\n");
		return 2;
	}
	printf("mapped at %p\n", smem);
	show_avail_mem("depois do map");

	phase_start("escrever");
	
	int nints = SHMEM_SIZE/sizeof(int);
	int *ints = (int *) smem;
	
	write_ints(ints, nints);
		
	
 	show_avail_mem("depois de escrever");
	phase_start("unmap");
	
	munmap(smem, SHMEM_SIZE);
	show_avail_mem("depois do unmap");
	phase_start("terminar");
	return 0;
}
