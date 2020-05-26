#include <stdio.h>

#include <limits.h>
#include <assert.h>
#include "chrono.h"
#include <stdlib.h>

#include "tpool.h"
#include "cdl.h"

#define NITERS 50
#define NPROCS 4
#define NVALS 50000 

typedef struct range {
	int *start, *end;
	long count;
	cdl_t *cdl;
} range_t;


// used thread pool
static tpool_t pool;

void oper_func(void *arg) {
	range_t *range = (range_t*) arg;
	long partial = 0;
	for(int *curr = range->start; curr < range->end; curr++) {
		partial += *curr;
	}
	range->count = partial;
	cdl_signal_one(range->cdl);
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
	 
	range_t ranges[NPROCS];
	
	int *curr = vals;
	int range_size = nvals /NPROCS;
	
	cdl_t cdl;
	
	cdl_init(&cdl, NPROCS);
	
	for(int i=0; i < NPROCS; ++i) {
		 	
		ranges[i].start = curr;
		ranges[i].end = (curr + range_size > vals + nvals) ?  &vals[nvals] : curr + range_size;
		ranges[i].count = 0;
		ranges[i].cdl = &cdl;
		
		curr += range_size;
		
		tp_submit(&pool, oper_func, ranges + i);
	}
	
	cdl_wait_for_all(&cdl);
	
	// colecionar resultados
	long total = 0;
	for(int i=0; i < NPROCS; ++i) {
		total += ranges[i].count;	
	}
	
	cdl_destroy(&cdl);
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
	
	
	for(int i=0; i < 1000; ++i) {
		tp_init(&pool);
		test("sequential adder", seq_adder, vals, NVALS, NVALS);
		test("parallel adder", par_adder, vals, NVALS, NVALS);
		tp_destroy(&pool);
	}
	
	
	return 0;
}

