#include <unistd.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "sysutils.h"

int get_major_pf_count() {
	struct rusage usage;
	
	if (getrusage(RUSAGE_SELF, &usage) == -1) {
		return -1;
	}
	
	return usage.ru_majflt;
	
}

int get_minor_pf_count() {
	struct rusage usage;
	
	if (getrusage(RUSAGE_SELF, &usage) == -1) {
		return -1;
	}
	
	return usage.ru_minflt;
	
}
