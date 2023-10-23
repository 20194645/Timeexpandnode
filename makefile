CC = g++
CFLAGS = -g -p

all: b.o

b.o: main.o Point.o Shape.o TimeExpandedNode.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) -c $< -o $@

Point.o: Point.cpp
	$(CC) $(CFLAGS) -c $< -o $@

Shape.o: Shape.cpp
	$(CC) $(CFLAGS) -c $< -o $@

TimeExpandedNode.o: TimeExpandedNode.cpp
	$(CC) $(CFLAGS) -c $< -o $@

debug: b.o
	gdb b.o

clean:
	rm -f *.o

.PHONY: all debug clean

