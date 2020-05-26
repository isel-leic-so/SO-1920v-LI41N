all: padder tp_adder

padder: padder.c chrono.c 
	gcc -o padder -Wall padder.c chrono.c -pthread
	
tp_adder: tp_adder.c queue.c cdl.c tpool.c chrono.c
	gcc -o tp_adder -Wall tp_adder.c queue.c cdl.c chrono.c tpool.c -pthread

clean:
	rm -f tp_adder padder
	
