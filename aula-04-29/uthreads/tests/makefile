
static : simpletest.c	result_tests.c
	gcc -o simpletest -Wall -g  -static simpletest.c ../uthread/libuthread.a
	gcc -o result_tests -Wall -g  -static result_tests.c unit_tests/unity.c ../uthread/libuthread.a
	
dynamic:	
	gcc -o simpletest -Wall -g simpletest.c ../uthread/libuthread.so
	gcc -o result_tests -Wall -g result_tests.c unit_tests/unity.c ../uthread/libuthread.so
	
clean:
	rm -rf simpletest
