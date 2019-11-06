CC=gcc
CFLAGS=-g -Werror -Wall

all: core

clean:
	rm -f *.o core

.PHONY: all clean

core: core.c queue.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

queue.o: queue.c
	$(CC) $(CFLAGS) -c -o $@ $^
