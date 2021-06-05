CC=gcc
CFLAGS=-O3 -Wall -Wextra -Wpedantic -pedantic-errors -Wfatal-errors -Werror -Wcast-align -Wunused -Wshadow -Wcast-qual  -Wno-missing-braces -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined
LDLIBS=-lm #Math Library
main: 
	$(CC) $(CFLAGS) src/main.c -o build/a.out $(LDLIBS)
clean:
	rm -rf a.out
