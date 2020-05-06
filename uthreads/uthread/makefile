all: libuthread.so libuthread.a

libuthread.so: uthread.o event.o semaphore.o  ctxswitch.o
	gcc -o libuthread.so -shared uthread.o event.o semaphore.o ctxswitch.o

libuthread.a: uthread_nogot.o event_nogot.o semaphore_nogot.o ctxswitch.o
	ar rcv libuthread.a uthread_nogot.o event_nogot.o semaphore_nogot.o ctxswitch.o

uthread.o : uthread.c
	gcc -c -g -fPIC -Wall uthread.c
	
uthread_nogot.o : uthread.c
	gcc -c -o uthread_nogot.o -g -Wall uthread.c
	
ctxswitch.o: ctxswitch.s
	as -o ctxswitch.o -c ctxswitch.s


#syncronizers
event.o: event.c
	gcc -c -g -fPIC -Wall  event.c 
	
semaphore.o: semaphore.c
	gcc -c -g -fPIC -Wall  semaphore.c 


event_nogot.o: event.c
	gcc -c -g -o event_nogot.o -Wall  event.c 
	
semaphore_nogot.o: semaphore.c
	gcc -c -g -o semaphore_nogot.o -Wall  semaphore.c 


		
clean:
	rm -rf *.o *.so *.a

	
