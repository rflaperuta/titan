CC=gcc
override CFLAGS+=-std=c99 -Wall -g
PREFIX=/usr/
LIBS=-lcrypto -lsqlite3
PROG=titan
OBJS=$(patsubst %.c, %.o, $(wildcard *.c))
HEADERS=$(wildcard *.h)

all: $(PROG)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f $(PROG)
