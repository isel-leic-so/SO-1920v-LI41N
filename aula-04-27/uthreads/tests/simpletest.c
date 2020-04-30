// SimpleTest.cpp : Defines the entry point for the console application.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/uthread.h"



/////////////////////////////////////////////
//
// CCISEL 
// 2007-2019
//
// UThread    Library First    Test
//
// Jorge Martins, 2019
////////////////////////////////////////////
#define DEBUG

#define MAX_THREADS 10



///////////////////////////////////////////////////////////////
//															 //
// Test 1: N threads, each one printing its number M times //
//															 //
///////////////////////////////////////////////////////////////

ULONG Test1_Count;


LONG Test1_Thread (UT_ARGUMENT Argument) {
	UCHAR Char;
	ULONG Index;
	Char = (UCHAR) (ULONG) Argument;	
	 
	for (Index = 0; Index < 100; ++Index) {
	    putchar(Char); fflush(stdout);
		 
	    if ((rand() % 4) == 0) {
			usleep(100000); 
		    UtYield();
	    }	 
    }
	++Test1_Count;
	return 0;
}

VOID Test1 ()  {
	ULONG Index;

	Test1_Count = 0; 

	printf("\n :: Test 1 - BEGIN :: \n\n");

	for (Index = 0; Index < MAX_THREADS; ++Index) {
		UtCreate(Test1_Thread, (UT_ARGUMENT) ('0' + Index));
	}   

	UtRun();

	_ASSERTE(Test1_Count == MAX_THREADS);
	printf("\n\n :: Test 1 - END :: \n");
}


// Test to run in single step

LONG Func1(UT_ARGUMENT arg) {
	HANDLE *pf2Handle = (HANDLE*) arg;
	printf("Start Func1\n");
	LONG f2Res = 0;
	
	BOOLEAN res  = UtTryGetThreadResult(*pf2Handle, &f2Res);
	printf("Func1: before UtYield res=%ld\n", f2Res);
	UtYield();
	
	while (!UtTryGetThreadResult(*pf2Handle, &f2Res)) {
		UtYield();
	}
	printf("Func1: after UtYield res=%ld\n", f2Res);
	printf("End Func1\n");
	return 0;
}

LONG Func2(UT_ARGUMENT arg) {
	printf("Func2\n");
	return 2;
}

VOID Test2() {
	printf("\n :: Test 2 - BEGIN :: \n\n");
	HANDLE thread2;
	
	UtCreate(Func1, &thread2);
	thread2 = UtCreate(Func2, NULL);
	UtRun();
	printf("\n\n :: Test 2 - END :: \n");
}



int main () {
	UtInit();
 
	Test2();
	
	 
	UtEnd();
	return 0;
}


