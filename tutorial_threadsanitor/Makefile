CC=clang
CFLAGS=-g -Wall -Wextra -Wpedantic -fsanitize=thread
LDFLAGS=-lpthread

example: example.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o example example.c

clean:
	rm -f example
