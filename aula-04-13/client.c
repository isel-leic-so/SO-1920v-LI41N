#include <stdio.h>

int count_calls();


int main() {
	int v = count_calls();
	
	printf("count_calls()=%d\n", v);
	
	v= count_calls();
	printf("count_calls()=%d\n", v);
	
	 
	return 0;
}
