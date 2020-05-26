#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "chrono.h"
#include "tpool2.h"
#include "cdl.h"

#define NITERS 50
#define NPROCS 4
#define NVALS 50000
 

tpool_t pool;

static int vals[NVALS];

void init_vals() {
	for(int i=0; i < NVALS; ++i) vals[i] = 1;
}

typedef struct _range {
	int *start;
	int *end;
	int  partial_sum;	
	cdl_t *cdl;
} range_t;

void oper_func(void *arg)  {
	//printf("pthread %ld\n", pthread_self());
	range_t *range = (range_t *) arg;
	long partial=0;
	for(int* curr = range->start; curr < range->end; ++curr) {
		partial  += *curr;
	}
	range->partial_sum = partial;
	cdl_signal_one(range->cdl);
	 
}

void range_init(range_t *range, int *start, int size, int *last, cdl_t *cdl) {
	
	range->start = start;
	range->end = (start + size) > last ? last : start + size;
	range->partial_sum = 0;	
	range->cdl = cdl;
}


long seq_sum(int vals[], int nvals) {
	long s = 0;
	for(int i= 0; i < nvals; ++i) s+= vals[i];
	return s;
}


long par_sum(int vals[], int nvals) {
	 
	range_t ranges[NPROCS];
	
	int range_size = NVALS / NPROCS;
	cdl_t cdl;
	
	cdl_init(&cdl, NPROCS);
 
	int total=0;
	int *curr = vals; 
 
	for(int i=0; i < NPROCS; ++i) {
		range_init(ranges+i, curr, range_size, vals + NVALS, &cdl);
		tp_submit( &pool, oper_func, ranges + i);
			 
		curr += range_size;
	}
	
	cdl_wait_for_all(&cdl);
	for(int i=0; i < NPROCS; ++i) {
		total += ranges[i].partial_sum;
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
		//printf("iter %d\n", i);
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
	tp_init(&pool);
	printf("init pool done!\n");
	
	 
	 
	test("sequencial test", seq_sum, vals, NVALS, NVALS);
	test("parallel test", par_sum, vals, NVALS, NVALS);
		
	 
	 
	return 0;
		 
}
	
