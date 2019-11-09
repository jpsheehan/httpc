CC=gcc
CFLAGS=-Werror -Wall -Wextra
DEBUG_CFLAGS=-g $(CFLAGS)
DEPS=server.o queue.o list.o buffer.o http.o dispatcher.o worker.o network.o http_headers.o stringmap.o logger.o

all: core test

clean:
	rm -f *.o core *.test

cloc:
	cloc *.c *.h

test: list.test

.PHONY: all clean cloc test

### BINARIES ###

core: core.c $(DEPS)
	$(CC) $(DEBUG_CFLAGS) -o $@ $^ -lpthread

### OBJECT FILES ###

queue.o: queue.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

server.o: server.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

list.o: list.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

buffer.o: buffer.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

http.o: http.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

dispatcher.o: dispatcher.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

worker.o: worker.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

network.o: network.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

http_headers.o: http_headers.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

stringmap.o: stringmap.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^

logger.o: logger.c
	$(CC) $(DEBUG_CFLAGS) -c -o $@ $^
	
# TESTS:
list.test: list_test.c list.o
	$(CC) $(DEBUG_CFLAGS) -o $@ $^