CC=gcc
CFLAGS=-g -Werror -Wall

all: core

clean:
	rm -f *.o core

.PHONY: all clean

core: core.c *.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

queue.o: queue.c
	$(CC) $(CFLAGS) -c -o $@ $^

server.o: server.c
	$(CC) $(CFLAGS) -c -o $@ $^

list.o: list.c
	$(CC) $(CFLAGS) -c -o $@ $^

	