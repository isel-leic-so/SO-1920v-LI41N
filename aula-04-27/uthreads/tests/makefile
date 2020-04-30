static : simpletest.c
	gcc -o simpletest -Wall -g  -static simpletest.c ../uthread/libuthread.a
	
dynamic:	
	gcc -o simpletest -Wall -g simpletest.c ../uthread/libuthread.so
	
clean:
	rm -rf simpletest
