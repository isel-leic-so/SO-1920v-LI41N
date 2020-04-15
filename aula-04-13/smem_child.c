
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

void write_ints(int *ints, int len, int val) {
	for(int i=0; i < len;++i) {
		ints[i] = val;
	}
}

int main(int argc, char *argv[]) {
 	
	 
	show_avail_mem("Inicio");
	phase_start("file map");
	
	void *smem = anon_shmem(SHMEM_SIZE);
	if (smem == NULL) {
		printf("error mapping the file\n");
		return 2;
	}
	
	printf("mapped at %p\n", smem);
	show_avail_mem("depois do map");

	phase_start("escrever");
	
	int nints = SHMEM_SIZE/sizeof(int);
	int *ints = (int *) smem;
	
	write_ints(ints, nints, 1);
	
	show_avail_mem("depois de escrever");
	phase_start("criar processo filho");
	
	int child, status;
	
	if ( (child = fork()) == 0) {
		printf("filho: sum ints = %ld\n", sum_ints(ints, nints));
		show_avail_mem("filho: depois de ler");
		phase_start("escrever");
		write_ints(ints, nints, 2);
		show_avail_mem("filho: depois de escrever");
		phase_start("terminar");
		munmap(ints, SHMEM_SIZE);
		exit(0);
	}
	else {
		waitpid(child, &status, 0);
	
		show_avail_mem("após filho");
		phase_start("ler");
		printf("sum ints = %ld\n", sum_ints(ints, nints));
		show_avail_mem("depois de ler");
		phase_start("terminar");
		munmap(ints, SHMEM_SIZE);
		show_avail_mem("depois do unmap");
		phase_start("terminar");
		return 0;
	}
}
