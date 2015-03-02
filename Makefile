CC=gcc
CFLAGS=-c -g -O3
LDFLAGS=-lelf
SOURCES=emulator.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=subleq
 
all:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
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
