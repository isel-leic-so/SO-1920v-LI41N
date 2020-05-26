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

#include "meminfo.h"
#include "sysutils.h"

  
static const char *names[] = {
	"MemTotal",
	"MemFree",
	"MemAvailable",
	"Cached",
	"Shmem",
	"Dirty"
};
int total_names = sizeof(names)/sizeof(names[0]);



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

