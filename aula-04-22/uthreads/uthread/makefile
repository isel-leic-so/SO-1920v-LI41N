all: libuthread.so libuthread.a

libuthread.so: uthread.o ctxswitch.o
	gcc -o libuthread.so -shared uthread.o ctxswitch.o

libuthread.a: uthread_nogot.o ctxswitch.o
	ar rcv libuthread.a uthread_nogot.o ctxswitch.o

uthread.o : uthread.c
	gcc -c -g -fPIC -Wall uthread.c
	
uthread_nogot.o : uthread.c
	gcc -c -o uthread_nogot.o -g -Wall uthread.c
	
ctxswitch.o: ctxswitch.s
	as -o ctxswitch.o -c ctxswitch.s

clean:
	rm -rf *.o *.so *.a

	
