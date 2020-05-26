
static: simpletest.c	result_tests.c quizz2.c quizz2a.c
	gcc -o simpletest -Wall -g  -static simpletest.c ../uthread/libuthread.a
	gcc -o result_tests -Wall -g  -static result_tests.c unit_tests/unity.c ../uthread/libuthread.a
	gcc -o quizz2 -Wall -g quizz2.c ../uthread/libuthread.a
	gcc -o quizz2a -Wall -g quizz2a.c ../uthread/libuthread.a
	
dynamic: simpletest.c	result_tests.c quizz2.c	quizz2a.c
	gcc -o simpletest -Wall -g simpletest.c ../uthread/libuthread.so
	gcc -o result_tests -Wall -g result_tests.c unit_tests/unity.c ../uthread/libuthread.so
		gcc -o quizz2 -Wall -g quizz2.c ../uthread/libuthread.so 
	gcc -o quizz2a -Wall -g quizz2a.c ../uthread/libuthread.so
	
clean:
	rm -rf simpletest
