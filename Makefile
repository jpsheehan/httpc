CC=gcc
CFLAGS=-g -Werror -Wall

all: core

clean:
	rm -f *.o core

.PHONY: all clean

core: core.c server.o queue.o list.o buffer.o http.o dispatcher.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

queue.o: queue.c
	$(CC) $(CFLAGS) -c -o $@ $^

server.o: server.c
	$(CC) $(CFLAGS) -c -o $@ $^

list.o: list.c
	$(CC) $(CFLAGS) -c -o $@ $^

buffer.o: buffer.c
	$(CC) $(CFLAGS) -c -o $@ $^

http.o: http.c
	$(CC) $(CFLAGS) -c -o $@ $^

dispatcher.o: dispatcher.c
	$(CC) $(CFLAGS) -c -o $@ $^

	