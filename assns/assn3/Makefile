# Assignment 1 - Template Makefile
CC = g++
LDFLAGS = -lpthread
CFLAGS =  -g -Wall -O0 `pkg-config --cflags --libs opencv4`
BINARY = 
HEADERS = threadhandler.h
SOURCES = threadhandler.cpp
OBJS = $(SOURCES:.cpp=.o)

all: $(BINARY)

.cpp.o:
	$(CC) -c -g -o $@ $^

sobel: $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) sobel.cpp -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

sobelref: $(HEADERS) $(OBJS)
	$(CC) sobelref.cpp -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

#$(BINARY): $(HEADERS) $(SOURCES) $(OBJS)
#	$(CC) $(BINARY).cpp -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f sobel sobelref $(OBJS) $(BINARY:=.o) 
