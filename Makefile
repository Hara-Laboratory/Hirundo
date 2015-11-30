CC=$(HOME)/toolchains/bin/gcc
CFLAGS=-c -g -O3 -Wall
LDFLAGS=-lelf
SOURCES=./src/emulator.c ./src/faultInjector.c ./src/listMem.c ./src/listTrace.c# ./src/list.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=subleq
 
all:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c Makefile
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS) *~ dump 

run:
	./$(EXECUTABLE)

test:
	./$(EXECUTABLE) test0

test-:
	./$(EXECUTABLE) test1

test--:
	./$(EXECUTABLE) test2
