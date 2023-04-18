CC=clang
CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic

boocat: Boocat.c
	$(CC) $(CFLAGS) Boocat.c -o boocat

format:
	clang-format -i Boocat.c

clean:
	rm -f boocat