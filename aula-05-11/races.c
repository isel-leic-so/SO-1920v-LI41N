
// Use a semaphore to protect critical section 

#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>


#define NTHREADS 2
#define NITERS 1000000

static  int total=0;

sem_t lock;

void *oper_func(void *arg)  {
	for(int i=0; i < NITERS; ++i) {
		// total++;
		sem_wait(&lock);
		int aux = total;
		aux = aux+1;
		total = aux;
		sem_post(&lock);
		
	}
	return NULL;;
}

int main() {
	
	pthread_t threads[NTHREADS];
	
	sem_init(&lock, 0, 1);
	while(1) {
		total=0;
		for(int i=0; i < NTHREADS; ++i) {
			if (pthread_create(threads+i, NULL, oper_func, NULL) != 0) {
				perror("error creating thread");
				return 1;
			}
		}
		
		for(int i=0; i < NTHREADS; ++i) {
			if (pthread_join(threads[i], NULL) != 0) {
				perror("error joining thread");
				return 1;
			}
		}
		
		printf("total count=%d\n", total);
		
		assert(total == NITERS*NTHREADS);
	}
	return 0;
		 
}
	
	
	
