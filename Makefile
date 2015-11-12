CFLAGS=-std=c11 -Wall -Wextra -pedantic  -Werror -Wwrite-strings -Wfloat-equal -Waggregate-return -Winline  -Wstack-usage=1024

.PHONY: clean

hangman: hangman.o -lncurses

debug: CFLAGS+=-g
debug: hangman

clean:
	-rm *.o
