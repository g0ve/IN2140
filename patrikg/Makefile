CC = gcc
CFLAGS = -Wall -Werror -Wpedantic -std=c99 -g

BINARIES = ruterdrift

all: $(BINARIES)

ruterdrift: ruterdrift.c binoper.c
	$(CC) $(CFLAGS) ruterdrift.c binoper.c -o ruterdrift

clean:
	rm -f $(BINARIES)
