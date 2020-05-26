#include <stdio.h>

#include <pthread.h>
#include <limits.h>
#include <assert.h>
#include "chrono.h"

#define NITERS 50
#define NPROCS 4
#define NVALS 50000 

typedef struct range {
	int *start, *end;
	long count;
} range_t;


void *oper_func(void *arg) {
	range_t *range = (range_t*) arg;
	long partial = 0;
	for(int *curr = range->start; curr < range->end; curr++) {
		partial += *curr;
	}
	range->count = partial;
	return NULL;	
}

int vals[NVALS];


static void init_vals() {
	for(int i=0; i < NVALS; ++i)
		vals[i] = 1;
}

long seq_adder(int vals[], int nvals) {
	long total=0;
	for(int i=0; i < nvals; ++i)
		total += vals[i];
	return total;
}

long par_adder(int vals[] , int nvals) {
	pthread_t threads[NPROCS];
	range_t ranges[NPROCS];
	
	int *curr = vals;
	int range_size = nvals /NPROCS;
	
	for(int i=0; i < NPROCS; ++i) {
		 	
		ranges[i].start = curr;
		ranges[i].end = (curr + range_size > vals + nvals) ?  &vals[nvals] : curr + range_size;
		ranges[i].count = 0;
		curr += range_size;
		
		pthread_create(threads + i, NULL, oper_func, ranges + i);
	}
	
	// colecionar resultados
	long total = 0;
	for(int i=0; i < NPROCS; ++i) {
		 
		pthread_join(threads[i], NULL);
		total += ranges[i].count;	
	}
	
	return total;
}

void test(char *name,
		long (*sum_func)(int vals[], int nvals), 
		int vals[], 
		int nvals, 
		long expected) {
	long less_micros= LONG_MAX;
	long res;
	for(int i=0; i < NITERS; ++i) {
		chrono_t chrono = chrono_start();
		res = sum_func(vals, nvals);
		assert(expected == res);
	
		long micros = chrono_micros(chrono);
		
		if (micros < less_micros) 
			less_micros = micros;
	}
	printf("%s give result of %ld in %ld micros.\n", 
			name, res, less_micros);
	
}

int main() {
	init_vals();
	
	 
	test("sequential adder", seq_adder, vals, NVALS, NVALS);
	test("parallel adder", par_adder, vals, NVALS, NVALS);
	 
	return 0;
}

