#include <stdio.h>
#include <dlfcn.h>

#include "../utils/memutils.h"

typedef int (*count_calls_ptr)();


int main() {
	
	phase_start("load library");
	
	void *lib = dlopen("./libmath.so", RTLD_LAZY);
	if (lib == NULL) {
		perror("error loading library");
		return 1;
	}
	
	count_calls_ptr ct_func  = (count_calls_ptr) dlsym(lib, "count_calls");
	if (ct_func == NULL) {
		perror("error getting func address");
		return 1;
	}
	
	phase_start("load symbol");
	
 
	
	printf("count_calls()=%d\n", ct_func());
	
	int v= ct_func();
	printf("count_calls()=%d\n", v);
	
	int *total = (int *) dlsym(lib, "total");
	if (total == NULL) {
		perror("error getting total address");
		return 1;
	}
	
	printf("%d\n", *total);
	 
	dlclose(lib);
	return 0;
}
