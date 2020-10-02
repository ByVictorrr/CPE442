# Assignment 1 - Template Makefile
SHELL := /bin/bash
CC = g++
LDFLAGS = `pkg-config --cflags --libs opencv4` -pthread
CFLAGS =  -g # need a way to input all c files and headers
C_FILES = $(shell find . -name "*.cpp")
BINARY = sobel
OBJS = $(C_FILES:.cpp=.o)

all: .depend $(BINARY)

# This is to get the depencies of all the c files

.depend: $(C_FILES)
	rm -f ./.depend
	$(CC) -MM $^ > ./.depend;

include .depend

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINARY): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f $(OBJS) .depend


