#include <unistd.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <stdio.h>
#include <string.h>

#include "memutils.h"

  
static const char *names[] = {
	"MemTotal",
	"MemFree",
	"MemAvailable",
	"Cached",
	"Shmem",
	"Dirty"
};
int total_names = sizeof(names)/sizeof(names[0]);


int map_file(const char *path, file_map *fmap, size_t newsize) {
	int fd = open(path, O_CREAT | O_RDWR, 0666);
	if (fd == -1) return -1;
	
	// get the file size;
	struct stat stat;
	
	if (fstat(fd, &stat) == -1)
		return -2;
	
	if (newsize != 0) {
		if (ftruncate(fd, newsize) == -1) {
			perror("error truncating file!");
			return -3;
		}
	}
	else newsize = stat.st_size;
	
 
	void *base = mmap(NULL, newsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (base == (void*) -1) {
		perror("error mapping file!");
		return -4;
	}
		
	fmap->fd = fd;
	fmap->base = base;
	fmap->len = newsize;
	
	return 0;
						
}


void unmap_file(file_map *fmap) {
	munmap(fmap->base, fmap->len);
	close(fmap->fd);
}


int get_val_from_line(const char *line, const char *name) {
	int val= 0;
	char *s;
	
	if ((s=strchr(line, ':')) != NULL)
		sscanf(s, "%d", &val);
	return val;
}

static const char *get_name_from_line(const char *line) {
	for(int i=0; i < total_names; ++i) {
		if (strstr(line, names[i]) == line && line[strlen(names[i])] == ':') 
			return names[i];
	}
	return NULL;
}


int get_major_pf_count() {
	struct rusage usage;
	
	if (getrusage(RUSAGE_SELF, &usage) == -1) {
		fprintf(stderr, "error getting process page fault count\n");
		return 0;
	}
	
	return usage.ru_majflt;
	
}

int get_minor_pf_count() {
	struct rusage usage;
	
	if (getrusage(RUSAGE_SELF, &usage) == -1) {
		fprintf(stderr, "error getting process page fault count\n");
		return 0;
	}
	
	return usage.ru_minflt;
	
}

void show_avail_mem(const char *msg) {
	
	printf("%s\n", msg);
	FILE *meminfo = fopen("/proc/meminfo", "r");
	char line[512];
	
	if (meminfo == NULL) {
		fprintf(stderr, "error getting meminfo!\n");
		return;
	}
	while (fgets(line, 512, meminfo) != NULL) {
		const char *name;
		if ((name=get_name_from_line(line)) != NULL) {
			printf("%s", line);
		}
	}
	fclose(meminfo);
	
	printf("Hard page faults: %d\n", get_major_pf_count());
	printf("Soft page faults: %d\n", get_minor_pf_count());
	
}


void phase_start(const char* phaseName) {
	printf("Prima RETURN para %s\t[%u]\n", phaseName, getpid());
	getchar();
}

