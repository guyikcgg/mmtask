CC = gcc

OBJECTS=

HEADERS=mmtask.h

all: example

example: $(HEADERS) example/main.c
	$(CC) -o "example/example" $^

clean:
	rm $(OBJECTS)
