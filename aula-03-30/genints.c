/*---------------------------------------------------------------------
 * This program used mmap mechanism to create 
 * a  large binary file of integers in an efficient way
 * Build:
 * 		gcc -o genints -Wall genints.c ../utils/memutils.c
 * 
 * Isel, 2020
 *-------------------------------------------------------------------*/

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include "../utils/memutils.h"



int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: intfile <size> \n");
		return 1;
	}
	
	
	int size = atoi(argv[1]);
	if (size==0) {
		printf("invalid size number!\n");
		return 1;
	}
	
 
	file_map fmap;
    int res;
	
	if ( (res = map_file("intfile.dat", &fmap, size*sizeof(int) )) < 0) {
		printf("error %d mapping file!\n", res);
		return 1;
	}
	int *ints = (int *) fmap.base;
	
	for(int i=0; i < size; ++i) {
		ints[i] = 1;
	}
	
	unmap_file(&fmap);
	return 0;
}
