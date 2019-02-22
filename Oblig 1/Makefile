CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -g

BINARIES = ruterdrift

all: $(BINARIES)

ruterdrift: ruterdrift.c
	$(CC) $(CFLAGS) ruterdrift.c -o ruterdrift

clean:
	rm -f $(BINARIES)