all: vrisc

vrisc: main.o cpu.o mem.o bus.o
	g++ -o vrisc main.o cpu.o mem.o bus.o

main.o: main.cc
	g++ -c main.cc

cpu.o: cpu.cc
	g++ -c cpu.cc

bus.o: bus.cc
	g++ -c bus.cc

mem.o: mem.cc
	g++ -c mem.cc

clean:
	rm *.o vrisc