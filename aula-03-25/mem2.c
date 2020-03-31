/*----------------------------------------------------------------
 *  This program dynamically allocs a big buffer
 *  using it for read and write before 
 *  create a child that uses the inherited buffer for read an write.
 * 
 *  For each step it show mem info stats
 * 
 * The program ilustrates the Copy On Write (COW) 
 * that occurrs on child creation
 *
 * Isel, 2020
 * 
 * Build:
 * 		gcc -o mem2 -Wall mem2.c ../utuls/memutils.c
 * 
 *---------------------------------------------------------------*/

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>


#include "../utils/memutils.h"

typedef unsigned char byte;
typedef unsigned long ulong;



byte *data;
int size;


ulong read_data() {
	ulong sum = 0;
	ulong i;

	for (i = 0; i < size; ++i) {
		sum += data[i];
		 
	}
	return sum;
}

 
void write_data(int v) {
	ulong i;
	for (i = 0; i < size; ++i)
		data[i] = v;
}
 


int main(int argc, char *argv[])
{
	int child;
	int status;
	
	
	if (argc != 2) {
		printf("usage: mem2 <size>\n");
		return 1;
	}
	
	show_avail_mem("Start");
	phase_start("do malloc");
	
	data = (byte*) malloc(size = atoi(argv[1]));
	
	show_avail_mem("After malloc");
	phase_start("read data");

	read_data();
	
	show_avail_mem("After read"); 
	phase_start("do write");
		
	write_data(1);
		
	show_avail_mem("after write");
	phase_start("create child");
	
	if ((child = fork()) == 0) {
		
		show_avail_mem("Start (of child)");
		phase_start("do read in child");
		
		read_data();
		
		show_avail_mem("after read (in child)");
		phase_start("do write in child");
		
		write_data(2);
		
		show_avail_mem("after write (in child)");
		phase_start("terminate child");
		exit(0);
	}

	waitpid(child, &status, 0);
	
	show_avail_mem("After child termination");
	phase_start("termination");
	return 0;
}
