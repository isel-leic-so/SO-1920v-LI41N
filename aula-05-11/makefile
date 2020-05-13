all: padder races races2


races: races.c
	gcc -o races -Wall races.c -pthread

races2: races2.c
	gcc -o races2 -Wall races2.c -pthread
	
padder:	padder.c chrono.c
	gcc -o padder -Wall padder.c chrono.c -pthread

clean:
	rm -f padder   races

	
	
