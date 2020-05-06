#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/uthread.h"

#include "queue.h"


#define MAX_LINE 512

PQUEUE qreader, qfilter;


void reader(UT_ARGUMENT  _filename) {
	const char* filename = (const char*) _filename;
	
	char line[MAX_LINE];
	FILE *file = fopen(filename, "r");
	
	if (file == NULL) {
		queue_put(qreader, NULL);
		return;
	}
	
	while(fgets(line, MAX_LINE, file) != NULL) {
		queue_put(qreader, strdup(line));
		//printf("reader: a new line!\n");
	}
	queue_put(qreader, NULL);
	fclose(file);
	
}


void filter(UT_ARGUMENT  _word) {
	const char *word = (const char*) _word;
	
	
	char *line;
	 
	
	while((line = queue_get(qreader)) != NULL) {
		if (strstr(line, word)) {
			queue_put(qfilter, line);
			// printf("filter: a new line!\n");
		}
	}
	queue_put(qfilter, NULL);
	
}

void presenter() {
	char *line;
	int count = 0;
	
	while((line = queue_get(qfilter)) != NULL) {
		printf("%s", line);
		free(line);
		count++;
	}
	printf("%d occurrences found!\n", count);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("usage: pgrep <file> <word>\n");
		return 1;
	}
	
	QUEUE _qreader, _qfilter;
	queue_init(&_qreader, 1);
	queue_init(&_qfilter, 1);
	
	qreader = &_qreader;
	qfilter = &_qfilter;
	
	ut_init();
	
	ut_create(reader, argv[1]);
	ut_create(filter, argv[2]);
	ut_create(presenter, NULL);
	
	ut_run();
	
	ut_end();
	
	return 0;
}











		
