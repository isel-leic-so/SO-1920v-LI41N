#include <stdio.h>


extern int total;

int count_calls();


int main() {
	int v = count_calls();
	
	printf("count_calls()=%d\n", v);
	
	v= count_calls();
	printf("count_calls()=%d\n", v);
	
	int x = total;
	printf("total=%d\n", x);
	 
	return 0;
}
