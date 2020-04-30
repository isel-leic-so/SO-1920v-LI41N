// SimpleTest.cpp : Defines the entry point for the console application.
//

#include <stdbool.h>
#include <string.h>

#include "unit_tests/tests.h"


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/uthread.h"


LONG Func1(UT_ARGUMENT arg) {
	HANDLE *pf2Handle = (HANDLE*) arg;
	printf("Start Func1\n");
	LONG myResult = 0;
	
	BOOLEAN res =  UtGetThreadResult(*pf2Handle, &myResult);
	 
	if (!res)  
		 printf("Error getting reult in Thread1\n");
	printf("End Func1\n");
	return myResult + 1;
}

LONG Func2(UT_ARGUMENT arg) {
	printf("Func2\n");
	return 2;
}

VOID Test2() {
	UtInit();
	printf("\n :: Test 2 - BEGIN :: \n\n");
	HANDLE thread1, thread2;
	
	thread1 = UtCreate(Func1, &thread2);
	thread2 = UtCreate(Func2, NULL);
	UtRun();
	
	printf("\n\n :: Test 2 - END :: \n");
	
	LONG expected_result = 3, result;
	
	TEST_ASSERT_TRUE(UtGetThreadResult(thread1, &result));
	TEST_ASSERT_EQUAL_INT64(expected_result, result);
	
	UtEnd();
}



int main() {

	
	BEGIN_TESTS();
	
	RUN_TEST(Test2);
	 
	END_TESTS();

	return 0;
}
