CC=g++
FLAGS=-std=c++11 -Wall -O3
INCLUDES=include

all: bin/libgop.dll bin/test.o
	$(CC) $(FLAGS) -Lbin -lgop bin/test.o -o bin/test

bin/test.o: src/test.cpp
	$(CC) $(FLAGS) -I$(INCLUDES) -c src/test.cpp -o bin/test.o	

bin/libgop.dll: bin/gop.o
	$(CC) $(FLAGS) -shared bin/gop.o -o bin/libgop.dll	

bin/gop.o: src/gop.cpp include/gop.hpp
	$(CC) $(FLAGS) -I$(INCLUDES) -c src/gop.cpp -o bin/gop.o

clean:
	rm -f bin/*.o bin/*.dll bin/*.exe bin/test