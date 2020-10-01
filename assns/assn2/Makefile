# Assignment 1 - Template Makefile
CC = g++
LDFLAGS = 
CFLAGS =  -g -Wall -O0 `pkg-config --cflags --libs opencv4`
BINARY = sobel3b3
HEADERS = 
SOURCES = 
OBJS = $(SOURCES:.c=.o)

all: $(BINARY)

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $^

sobel: $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) sobel.cpp -o $@ $(CFLAGS) $(LDFLAGS)
	
$(BINARY): $(HEADERS) $(SOURCES) $(OBJS)
	$(CC) $(BINARY).cpp -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(BINARY:.cpp=.o)


