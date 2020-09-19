# Assignment 1 - Template Makefile
# for C
CC?= g++
CFLAGS?= -g
LDFLAGS?=
SOURCES?= 
TARGET?=
PROGS=$(wildcard *.c)

all: $(PROGS) .depend $(TARGET)

# This is to get the depencies of all the c files
.depend: $(SOURCES)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend;

include .depend

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(TARGET) -o $(TARGET) $(LDFLAGS)

.c:
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(PROGS:.c=) *.o .depend




