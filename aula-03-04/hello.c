#include <stdio.h>
#include <unistd.h>

int main() {
	printf("Hello, world!\n");
	
	fprintf(stderr, "my id is: %d\n", getpid());
	
	printf("Press return to terminate... ");
	getchar();
	return 0;
}
	
