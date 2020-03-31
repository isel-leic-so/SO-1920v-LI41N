// texec.cpp : Defines the entry point for the console application.
// A simple illustration of the separated virtual address spaces for diferent processes
// test launching two instances of the program
//
// Isel, 2020

#include "stdio.h"

int a[1024];

int main(int argc, char* argv[])
{
	
	printf("main address = %p\n", main);
	printf("a address = %p\n", a);
	printf("press return to start...");
	getchar();
	
	printf("a[0]=%d\n", a[0]);
	printf("press return to continue...");
	getchar();
	a[0] = 1;
	printf("a[0]=%d after set by process\n", a[0]);
	printf("press return to end...");
	getchar();
	return 0;
}

