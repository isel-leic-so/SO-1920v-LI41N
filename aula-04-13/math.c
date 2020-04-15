#include <stdio.h>

int total=0;

int count_calls() {
	return ++total;
}

int get_count_calls() {
	return total;
}

long add(int i1, int i2) {
	count_calls();
	return i1 + i2;
}


/*
 * 		call L1
 *   L1:pop eax
 */
