# Assignment 1 - Template Makefile
CC = g++
LDFLAGS = -lpthread
CFLAGS =  -g -Wall -O0 `pkg-config --cflags --libs opencv` -mfpu=neon -mcpu=Cortex-A8 #`pkg-config --libs --cflags libperf`
BINARY = sobel
C_FILES = $(shell find . -name "*.cpp")
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
