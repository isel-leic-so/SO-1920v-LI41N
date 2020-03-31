
/*----------------------------------------------------------------
 *  This program uses a large initialized  buffer
 *  using it for read and write. 
 *   
 * 
 *  For each step it show mem info stats
 * 
 * Isel, 2020
 * 
 * Build:
 * 		gcc -o mem -Wall mem.c ../utils/memutils.c
 * 
 *---------------------------------------------------------------*/
 
#include <stdio.h>
#include "../utils/memutils.h"
 


#define DATALEN (100*1024*1024)

static int data[DATALEN] = {1};
 
ulong read_data() {
	ulong sum = 0;
	ulong i, c=0;

	for (i = 0; i < DATALEN; ++i) {
		sum += data[i];
		c++;
	}
	printf("count=%ld\n", c);
	return sum;
}


void write_data(int v) {
	ulong i;
	for (i = 0; i < DATALEN; ++i)
		data[i] = v;
}

 
int main()
{
	show_avail_mem("Inicio");
	phase_start("ler");
	
	printf("data sum=%ld\n", read_data());
	show_avail_mem("Depois de ler");
	phase_start("escrever");
	write_data(2);
	show_avail_mem("Depois de escrever");
	phase_start("terminar");
	return 0;
}
