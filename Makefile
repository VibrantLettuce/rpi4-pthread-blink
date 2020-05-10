CC=gcc
CFLAGS=-I.
DEPS=pin_state.h
LDLIBS=-lgpiod -lpthread

all: blink

blink: main.c $(DEPS)
	$(CC) -o $@ $< $(CFLAGS) $(LDLIBS)
