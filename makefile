CC = gcc

OBJECTS=

HEADERS=mmtask.h

all: example

example: $(HEADERS) example/linux.c
	$(CC) -o "example/example" $^

clean:
	rm $(OBJECTS)
