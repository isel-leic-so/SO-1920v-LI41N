
dynamic:	
	gcc -o simpletest -Wall -g simpletest.c ../uthread/libuthread.so
	gcc -o event_test -Wall -g event_test.c ../uthread/libuthread.so
	gcc -o pgrep -g -Wall pgrep.c queue.c ../uthread/libuthread.so
	
	
static : simpletest.c	result_tests.c
	gcc -o simpletest -Wall -g  -static simpletest.c ../uthread/libuthread.a
	gcc -o event_test -Wall -g event_test.c ../uthread/libuthread.a
	gcc -o pgrep -g -Wall pgrep.c queue.c ../uthread/libuthread.a
	
clean:
	rm -rf simpletest event_test
