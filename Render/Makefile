all:
	g++ *.cpp -o render -lpng -lpthread -Ofast -Wall

gdb:
	g++ *.cpp -o render -lpng -lpthread -O0 -Wall -g

gprof:
	g++ *.cpp -o render -lpng -lpthread -Ofast -pg

clean:
	rm *~ render
