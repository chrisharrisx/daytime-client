CC = gcc
CFLAGS = -std=c99 -g -Wall
SOURCES = client.c
OBJECTS = $(SOURCES:.c=.o)
MAIN = client

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(MAIN)

clean:
	rm $(OBJECTS) $(MAIN)