# Assignment 1 - Template Makefile
SHELL := /bin/bash
CC = g++
LDFLAGS = 
CFLAGS =  -g -I/usr/local/opencv4
# need a way to input all c files and headers
C_FILES = $(shell find . -name "*.cpp")
BINARY = sobel
OBJS = $(C_FILES:.cpp=.o)

all: .depend $(BINARY)

# This is to get the depencies of all the c files

.depend: $(C_FILES)
	rm -f ./.depend
	$(CC) -MM $^ > ./.depend;

include .depend


$(BINARY): $(OBJS)
	$(CC) $(C_FLAGS) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f $(OBJS) .depend


