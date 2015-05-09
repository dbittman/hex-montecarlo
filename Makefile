CXXFLAGS=-O3
hex: main.o graph.o
	g++ main.o graph.o -o hex

main.o: main.cpp graph.h hex.h

graph.o: graph.cpp graph.h

clean:
	-rm hex *.o
