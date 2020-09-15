# Assignment 1 - Template Makefile
SHELL := /bin/bash
CC = gcc
LDFLAGS = 
CFLAGS =  -g
# need a way to input all c files and headers
C_FILES = $(shell find . -name "*.c")
BINARY = main
OBJS = $(C_FILES:.c=.o)

all: .depend $(BINARY)

# This is to get the depencies of all the c files

.depend: $(C_FILES)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend;
	echo $(OBJS)
	

include .depend


$(BINARY): $(OBJS)
	echo $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@


clean:
	rm -f *.o .depend


