CC = gcc
CFLAGS = -std=c99
FILES = client.c
OBJECTS = client

all:
	$(CC) $(CFLAGS) $(FILES) -o $(OBJECTS)

clean:
	rm $(OBJECTS)