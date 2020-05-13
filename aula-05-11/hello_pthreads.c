#include <pthread.h>

#include <stdio.h>


void *func(void *arg) {
	char * creator_name = (char*) arg;
	printf("hello, %s\n", creator_name);
	
	return "child";
}


int main() {
	pthread_t thread;
	
	int res = pthread_create(&thread, NULL, func, "creator");
	if (res != 0) {
		perror("error creating pthread");
		return 1;
	}
	
	char * child_name;
	
	if (pthread_join(thread, (void *) &child_name) != 0) {
		perror("error joining with thread");
		return 1;
	}
	
	printf("hello from %s\n", child_name);
	
	return 0;
}
	
	
