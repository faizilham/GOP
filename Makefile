CC=g++
FLAGS=-std=c++11 -Wall
INCLUDES=include

all: bin/gop.o bin/test.o
	$(CC) $(FLAGS) bin/test.o bin/gop.o -o bin/test

bin/test.o:
	$(CC) $(FLAGS) -I$(INCLUDES) -c src/test.cpp -o bin/test.o	

bin/gop.o: src/gop.cpp include/gop.hpp
	$(CC) $(FLAGS) -I$(INCLUDES) -c src/gop.cpp -o bin/gop.o

clean:
	rm -f bin/*.o bin/*.dll bin/*.exe bin/test