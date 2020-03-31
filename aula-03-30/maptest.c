
/*-----------------------------------------------------------------
 * This program illustrates the behaviour of a file map opertaion
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

#include "../utils/memutils.h"


long sum_ints(int *ints) {
	long sum =0;
	int *ints = fmap.base;
	for(int i=0; i < fmap.len/sizeof(int);++i) {
		sum += ints[i];
	}
	return sum;
}

int main(int argc, char *argv[]) {
	if (argc <2 || argc > 3) {
		printf("usage: maptest <file>  [<size>]\n");
		return 1;
	}
	
	file_map fmap;
	int size = 0;
	
	if (argc == 3)  
		size = atoi(argv[2]);
	printf("size=%d\n", size);
	show_avail_mem("Inicio");
	phase_start("file map");
	
	int res = map_file(argv[1], &fmap, size);
	if (res != 0) {
		printf("error %d mapping the file\n", res);
		return 2;
	}
	printf("mapped at %p\n", fmap.base);
	show_avail_mem("depois do map");

	phase_start("read");
	
	long sum = sum_ints((int *) fmap.base);
	printf("sum=%ld\n", sum);
	
	show_avail_mem("depois do read");
	phase_start("terminar");
	
	// decomment the next line to change the mapped file
	//ints[0] = 10;
	unmap_file(&fmap);
	return 0;
}
