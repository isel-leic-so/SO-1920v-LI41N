#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define PAGESIZE 4096

long getPfState(int pid, unsigned long startAddr, unsigned long endAddr, long res[] ) {
	printf("pid=%d,start=%lx,end=%lx\n", pid, startAddr, endAddr);
	char pagemap[128];
	sprintf(pagemap, "/proc/%d/pagemap", pid);
	 
	int pagemap_fd = open(pagemap, O_RDONLY);
	 
	if (pagemap_fd == -1) {
		fprintf(stderr, "error opening pagemap file!\n");
		return -1;
	}
	
	unsigned long startPage = startAddr / PAGESIZE;
	unsigned long lastPage = endAddr /PAGESIZE;
	
	int totalPages = lastPage - startPage;
	
	// seek to the end
	/*	
	long result = lseek(pagemap_fd, 0,  SEEK_END);
	if (result == -1) {
		perror("error getting pagemap filesize");
		return -1;
	}
	printf("pagemap file size is %ld\n", result);
	*/
	// see to first page
	if (lseek(pagemap_fd,  (off_t) startPage*8, SEEK_SET) == -1) {
		perror("error seeking pagemap file");
		close(pagemap_fd);
		return -1;
	}
	
	if (read(pagemap_fd, res, 8*totalPages) == -1) {
		perror("error reading pagemap file");
		printf("total pages=%d\n", totalPages);
		close(pagemap_fd);
		return -1;
	}
	 
	close(pagemap_fd);
	
	return totalPages;
	
}

long r[10000000];

void show_pinfo(unsigned long p) {
	if (p & 0x8000000000000000) {
		printf("PR SW MF XM SD PFN\n");
		printf("            ");
		printf("%2d ", p & 0x8000000000000000 ? 1 : 0);
		printf("%2d ", p & 0x4000000000000000 ? 1 : 0);
		printf("%2d ", p & 0x2000000000000000 ? 1 : 0);
		printf("%2d ", p & 0x800000000000000  ? 1 : 0);
		printf("%2d ", p & 0x400000000000000  ? 1 : 0);
		printf("%-lX\n", p & 0xfffffffffffff3);
	}
	else {
		printf("PR SW MF XM SD SWO    ST\n");
			printf("            ");
		printf("%2d ", p & 0x8000000000000000 ? 1 : 0);
		printf("%2d ", p & 0x4000000000000000 ? 1 : 0);
		printf("%2d ", p & 0x2000000000000000 ? 1 : 0);
		printf("%2d ", p & 0x80000000000000  ? 1 : 0);
		printf("%2d ", p & 0x40000000000000  ? 1 : 0);
		printf("%8lX,%-ld\n", (p & 0x3fffffffffffff) >> 5, p & 0x1F);
	}
}

 
int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("usage: pframe <pid> <startAddress> <enddress> \n");
		return 1;
	}
	

	long total;
	
	if ((total=getPfState(atoi(argv[1]), strtol(argv[2], NULL,16), strtol(argv[3],NULL,16), r)) > 0) {
		for(long i=0; i < total; ++i) {
			printf("page[%3ld] = ", i);show_pinfo( r[i]);
			//printf("%lX\n", r[i]);
		}
	}
	
	return 0;
	
}
