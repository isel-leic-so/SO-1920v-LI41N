all: libmath.so client clientdyn smem_child smem_writer smem_reader

libmath.so: math.o
	gcc -o libmath.so -shared math.o

math.o: math.c
	gcc -o math.o -c -fpic -Wall math.c
	

client:	client.o libmath.so
	gcc -o client client.o ./libmath.so 
	
client.o: client.c
	gcc -o client.o -c -Wall client.c 

clientdyn: clientdyn.o
	gcc -o clientdyn clientdyn.o ../utils/memutils.c  -ldl -lrt
	
clientdyn.o: clientdyn.c  
	gcc -o clientdyn.o -Wall -c clientdyn.c 
	
smem_child: smem_child.c
	gcc -o smem_child -Wall smem_child.c ../utils/memutils.c  ../utils/memutils2.c -lrt

smem_reader: smem_reader.c
	gcc -o smem_reader -Wall smem_reader.c ../utils/memutils.c ../utils/memutils2.c -lrt

smem_writer: smem_writer.c
	gcc -o smem_writer -Wall smem_writer.c ../utils/memutils.c ../utils/memutils2.c -lrt
	
clean:
	rm *.so *.o client clientdyn smem_child smem_writer smem_reader
