#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned long ulong;

#define DATALEN (1024*1024*300)

static byte data[DATALEN] = { 1 };

void show_avail_mem(const char *msg) {
	FILE *meminfo = fopen("/proc/meminfo", "r");
	char line[512];
	int free;
	
	if (meminfo == NULL) {
		fprintf(stderr, "error getting meminfo!\n");
		return;
	}
	while (fgets(line, 512, meminfo) != NULL) {
		
		if (strstr(line, "MemAvailable:") == line) {
			//printf("MemAvailable found!\n");
			if (sscanf(line, "MemAvailable:%d", &free) == 1) {
				printf("%s: Found %dKb of available memory\n", msg, free);
				break;
			}
		}
	 
	}
	fclose(meminfo);
}

ulong read_data() {
	ulong sum = 0;
	ulong i;

	for (i = 0; i < DATALEN; ++i)
		sum += data[i];
	return sum;
}

void write_data(int v) {
	ulong i;
	for (i = 0; i < DATALEN; ++i)
		data[i] = v;
}

void phase_start(const char* phaseName) {
	printf("Prima RETURN para %s\t[%u]\n", phaseName, getpid());
	getchar();
}

int main()
{
	show_avail_mem("Inicio");
	phase_start("ler");
	read_data();
	show_avail_mem("Depois de ler");
	phase_start("escrever");
	write_data(2);
	show_avail_mem("Depois de escrever");
	phase_start("terminar");
	return 0;
}
